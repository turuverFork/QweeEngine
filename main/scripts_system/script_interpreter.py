import re
import sys
from typing import Dict, List, Tuple, Any

class ScriptInterpreter:
    def __init__(self):
        self.window_config = {"width": 1280, "height": 720}
        self.scenes = {}
        self.current_scene = None
        self.objects = []
        self.scripts = {}
        self.generated_code = []
        self.variables = {}
        
    def parse_file(self, filename: str) -> str:
        """Parse script file and generate C code"""
        with open(filename, 'r', encoding='utf-8') as f:
            content = f.read()

        content = re.sub(r'#.*', '', content)

        sections = self._split_sections(content)

        if 'window' in sections:
            self._parse_window(sections['window'])

        for name, section in sections.items():
            if name.startswith('scene'):
                self._parse_scene(name, section)

        return self._generate_c_code()
    
    def _split_sections(self, content: str) -> Dict[str, str]:
        """Split content into named sections"""
        sections = {}
    
        pattern = r'(\w+)(?:\s*\[|\s*\{)(.*?)(?:\]|\})'
        
        matches = list(re.finditer(pattern, content, re.DOTALL))
        
        for match in matches:
            section_type = match.group(1)
            section_content = match.group(2).strip()

            if '(' in section_type:
                name_match = re.search(r'(\w+)\((\w+)\)', section_type)
                if name_match:
                    section_type = f"{name_match.group(1)}_{name_match.group(2)}"
            
            sections[section_type] = section_content
        
        return sections
    
    def _parse_window(self, content: str):
        """Parse window configuration"""
        lines = content.strip().split('\n')
        for line in lines:
            line = line.strip()
            if not line:
                continue
            match = re.match(r'(\w+)\s+(\d+)', line)
            if match:
                key, value = match.groups()
                self.window_config[key] = int(value)
    
    def _parse_scene(self, scene_name: str, content: str):
        """Parse a scene definition"""
        self.current_scene = scene_name
        self.scenes[scene_name] = {"objects": [], "scripts": []}

        lines = content.strip().split('\n')
        current_block = None
        block_content = []
        
        for line in lines:
            line = line.strip()
            if not line:
                continue

            if line.startswith('create.') or line.startswith('background'):
                if current_block:
                    self._process_block(current_block, block_content)
                    block_content = []
                
                if 'create script' in line:
                    current_block = 'script'

                    match = re.search(r'create script\s*\((\w+)\)', line)
                    if match:
                        current_block = f"script_{match.group(1)}"
                elif 'background' in line:
                    match = re.search(r'background\s*=\s*\((\d+),\s*(\d+),\s*(\d+)\)', line)
                    if match:
                        r, g, b = match.groups()
                        self.scenes[scene_name]["background"] = (int(r), int(g), int(b))
                else:
 
                    self._parse_object_creation(line, scene_name)
            elif current_block:
                block_content.append(line)
 
        if current_block and block_content:
            self._process_block(current_block, block_content)
    
    def _parse_object_creation(self, line: str, scene_name: str):
        """Parse object creation line"""
        match = re.match(r'create\.(obj|2d)\.(\w+)\(([^)]+)\)(?:\s*\{([^}]+)\})?', line)
        if not match:
            match = re.match(r'create\.(\w+)\(([^)]*)\)', line)
            if match:
                obj_type = match.group(1)
                params = match.group(2)
                
                obj = {
                    "type": obj_type,
                    "scene": scene_name,
                    "params": params,
                    "script": None
                }
                self.scenes[scene_name]["objects"].append(obj)
                self.objects.append(obj)
                return
        
        if match:
            obj_category = match.group(1) 
            obj_type = match.group(2)      
            params = match.group(3)        
            script_link = match.group(4) if match.group(4) else None
            
            obj = {
                "category": obj_category,
                "type": obj_type,
                "scene": scene_name,
                "params": params,
                "script": None
            }

            if script_link:
                script_match = re.search(r'lin\s*->\s*(\w+)', script_link)
                if script_match:
                    obj["script"] = script_match.group(1)
            
            self.scenes[scene_name]["objects"].append(obj)
            self.objects.append(obj)
    
    def _process_block(self, block_name: str, content: List[str]):
        """Process a code block (script)"""
        if block_name.startswith('script_'):
            script_name = block_name.replace('script_', '')
            self.scripts[script_name] = content
            if self.current_scene:
                self.scenes[self.current_scene]["scripts"].append(script_name)
    
    def _generate_c_code(self) -> str:
        """Generate C code from parsed data"""
        self.generated_code = []

        self.generated_code.append("""
#include "engine.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

// ==================== GLOBAL VARIABLES ====================
static GameState gameState = STATE_INTRO;
static int menuSelection = 0;
static float menuCooldown = 0.2f;
static bool gameStarted = false;
static bool gamePaused = false;
static bool gameOver = false;
static int score = 0;
static int kills = 0;
static float gameTime = 0;

// ==================== SCENE MANAGEMENT ====================
typedef enum {
    STATE_INTRO,
    STATE_MAIN_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAME_OVER
} GameState;

// ==================== GENERATED OBJECTS ====================
""")
        

        for obj in self.objects:
            if obj["category"] == "obj":
                obj_name = obj["type"]
                if obj["type"] == "player":
                    self.generated_code.append(f"static GameObject* {obj_name}Obj = NULL;")
                else:
                    self.generated_code.append(f"static GameObject* {obj_name}_{obj['scene']} = NULL;")

        self._generate_script_functions()

        self._generate_scene_init()

        self._generate_main_function()
        
        return '\n'.join(self.generated_code)
    
    def _generate_script_functions(self):
        """Generate C functions from scripts"""
        for script_name, script_lines in self.scripts.items():
            func_name = f"Script_{script_name}"
            
            self.generated_code.append(f"\n// ==================== {script_name.upper()} SCRIPT ====================")
            self.generated_code.append(f"void {func_name}(float deltaTime)")
            self.generated_code.append("{")

            for line in script_lines:
                c_line = self._convert_script_to_c(line)
                if c_line:
                    self.generated_code.append(f"    {c_line}")
            
            self.generated_code.append("}")
    
    def _convert_script_to_c(self, line: str) -> str:
        """Convert a script line to C code"""
        line = line.strip()
 
        match = re.match(r'#define\s+(\w+)\s*=\s*(.+)', line)
        if match:
            var_name, value = match.groups()
            self.variables[var_name] = value
            return f"float {var_name} = {value};"
 
        if 'if' in line:
            return self._convert_if_statement(line)
  
        if 'move' in line and 'to ->' in line:
            return self._convert_move_statement(line)

        if line.startswith('printf'):
            return self._convert_print_statement(line)

        match = re.match(r'(\w+)\s*([+-]?=)\s*(.+);', line)
        if match:
            var, op, value = match.groups()
            return f"{var} {op} {value};"
        match = re.match(r'(\w+)\(([^)]*)\);', line)
        if match:
            func, params = match.groups()
            return f"{func}({params});"

        if 'collision ->' in line:
            return self._convert_collision_check(line)

        if 'next.scene ->' in line:
            match = re.search(r'next\.scene\s*->\s*(\w+)', line)
            if match:
                scene = match.group(1)
                return f'gameState = STATE_PLAYING; // Switch to scene: {scene}'
        
        return line
    
    def _convert_if_statement(self, line: str) -> str:
        """Convert if statement to C"""
        
        if 'key_preseed' in line:

            match = re.search(r'key_code\s*==\s*(.+)', line)
            if match:
                keys = match.group(1)
                keys = keys.replace('||', '||')
                return f"if (IsKeyDown({keys})) {{"
        
        elif 'collision == true' in line:
            return "if (collision_detected) {"
        
        elif 'collision ->' in line:
            match = re.search(r'collision\s*->\s*(\w+)', line)
            if match:
                target = match.group(1)
                return f"if (CheckCollision(playerObj, {target}_{self.current_scene})) {{"
        
        elif 'button_pressed == true' in line:
            return "if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {"
        
        return line.replace(':', '{')
    
    def _convert_move_statement(self, line: str) -> str:
        """Convert move statement to C"""

        match = re.search(r'move\(([^)]+)\)\s*to\s*->\s*(\w+)', line)
        if match:
            speed, target = match.groups()
            return f"// Move towards {target} with speed {speed}"
        
        return line
    
    def _convert_print_statement(self, line: str) -> str:
        """Convert print statement to C"""

        match = re.search(r'printf\(([^)]+)\)', line)
        if match:
            value = match.group(1)
            return f'printf("{value}\\n");'
        
        return line
    
    def _convert_collision_check(self, line: str) -> str:
        """Convert collision check to C"""

        return "// Collision check placeholder"
    
    def _generate_scene_init(self):
        """Generate scene initialization code"""
        self.generated_code.append("""
// ==================== INITIALIZE GAME ====================
void InitGame()
{
    printf("=== GAME INITIALIZED ===\\n");
    
    // Initialize player physics settings
    PlayerPhysicsSettings* settings = GetPlayerSettings();
    if (settings) {
        settings->walkSpeed = 5.0f;
        settings->runSpeed = 10.0f;
        settings->jumpForce = 12.0f;
        settings->gravity = -25.0f;
        settings->playerHeight = 1.8f;
        settings->playerRadius = 0.5f;
    }
""")

        for scene_name, scene_data in self.scenes.items():
            self.generated_code.append(f"\n    // === {scene_name.upper()} OBJECTS ===")

            if "background" in scene_data:
                r, g, b = scene_data["background"]
                self.generated_code.append(f"    // Background color: ({r}, {g}, {b})")

            for obj in scene_data["objects"]:
                c_code = self._generate_object_creation_c(obj)
                if c_code:
                    self.generated_code.append(f"    {c_code}")
        
        self.generated_code.append("""
    printf("Game objects created\\n");
}

// ==================== UPDATE GAME ====================
void UpdateGame(float deltaTime)
{
    gameTime += deltaTime;
    
    switch (gameState) {
        case STATE_PLAYING:
            // Update player movement
            UpdateCameraControl();
            UpdatePlayerMovement(deltaTime);
            
            // Run object scripts
""")

        for scene_name, scene_data in self.scenes.items():
            for script_name in scene_data["scripts"]:
                self.generated_code.append(f"            Script_{script_name}(deltaTime);")
        
        self.generated_code.append("""
            break;
            
        case STATE_MAIN_MENU:
            // Menu logic here
            break;
    }
    
    UpdateEngine(deltaTime);
}

// ==================== RENDER GAME ====================
void RenderGame()
{
    BeginDrawing();
    
    switch (gameState) {
        case STATE_PLAYING:
            ClearBackground((Color){30, 30, 50, 255});
            
            BeginMode3D(*GetCamera());
            
            // Render all objects
            GameObject** objects = GetObjects();
            int* objectCount = GetObjectCount();
            
            for (int i = 0; i < *objectCount; i++) {
                if (objects[i] && objects[i]->isActive) {
                    DrawObject(objects[i]);
                }
            }
            
            EndMode3D();
            break;
            
        case STATE_MAIN_MENU:
            // Render menu
            break;
    }
    
    DrawFPS(10, 10);
    EndDrawing();
}
""")
    
    def _generate_object_creation_c(self, obj: Dict[str, Any]) -> str:
        """Generate C code for object creation"""
        obj_type = obj["type"]
        params = obj["params"]
        scene = obj["scene"]

        param_list = [p.strip() for p in params.split(',')] if params else []

        x = "0" if len(param_list) < 1 else param_list[0]
        y = "0" if len(param_list) < 2 else param_list[1]
        z = "0" if len(param_list) < 3 else param_list[2]
        
        if obj["category"] == "obj":
            if obj_type == "player":
                return f'playerObj = CreatePlayer("Player", {x}, {y}, {z}, true, true);'
            elif obj_type == "box" or obj_type == "eme":
                return f'{obj_type}_{scene} = CreateCube("{obj_type}", {x}, {y}, {z}, false, true, NULL, RED);'
            elif obj_type == "plane":
                color = "GREEN" if len(param_list) > 0 else "GRAY"
                return f'CreatePlane("Ground", 0, 0, 0, 100, 100, {color});'
        
        elif obj["category"] == "2d":
            if obj_type == "button":
                color_match = re.search(r'\((\d+),\s*(\d+),\s*(\d+)\)', params)
                if color_match:
                    r, g, b = color_match.groups()
                    return f'// 2D Button at ({x}, {y}) with color ({r}, {g}, {b})'
        
        return f'// Created {obj_type} at ({x}, {y}, {z})'
    
    def _generate_main_function(self):
        """Generate main function"""
        width = self.window_config.get("width", 1280)
        height = self.window_config.get("height", 720)
        
        self.generated_code.append(f"""
// ==================== MAIN FUNCTION ====================
int main(void)
{{
    // Initialize engine with configured window size
    InitEngine({width}, {height}, "QWEE Script Game", false);
    
    // Initialize game objects
    InitGame();
    
    // Set initial scene
    gameState = STATE_MAIN_MENU;
    
    printf("\\n=== GAME STARTED ===\\n");
    printf("Window: {width}x{height}\\n");
    
    // Main game loop
    while (!WindowShouldClose())
    {{
        float dt = GetDeltaTime();
        if (dt > 0.1f) dt = 0.1f;
        
        UpdateGame(dt);
        RenderGame();
        
        // Exit on ESC
        if (IsKeyPressed(KEY_ESCAPE))
            break;
    }}
    
    // Cleanup
    CloseEngine();
    printf("\\n=== GAME ENDED ===\\n");
    
    return 0;
}}
""")

def main():
    """Main function"""
    if len(sys.argv) < 2:
        print("Usage: python script_interpreter.py <input_file> [output_file]")
        print("Example: python script_interpreter.py game.qwee script.c")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2] if len(sys.argv) > 2 else "script.c"
    
    interpreter = ScriptInterpreter()
    
    try:
        c_code = interpreter.parse_file(input_file)
        
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write(c_code)
        
        print(f"Successfully generated {output_file}")
        print(f"Scenes: {list(interpreter.scenes.keys())}")
        print(f"Objects: {len(interpreter.objects)}")
        print(f"Scripts: {list(interpreter.scripts.keys())}")
        
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()