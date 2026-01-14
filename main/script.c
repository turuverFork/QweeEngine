#include "engine.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define MAX_ENEMIES 50
#define MAX_BULLETS 200
#define MAX_POWERUPS 20
#define MAX_AMMO 30
#define BULLET_SPEED 70.0f
#define ENEMY_SPEED_MIN 1.5f
#define ENEMY_SPEED_MAX 4.0f
#define PLAYER_HEALTH 150
#define PLAYER_SPEED 8.0f
#define PLAYER_RUN_SPEED 16.0f
#define MOUSE_SENSITIVITY 0.003f
#define ARENA_SIZE 80.0f
#define ARENA_WALL_HEIGHT 10.0f

typedef struct {
    Vector3 position;
    Vector3 velocity;
    bool active;
    float lifetime;
    Color color;
    float size;
} Bullet3D;

typedef struct {
    Vector3 position;
    Vector3 velocity;
    bool active;
    int health;
    float size;
    Color color;
    float rotation;
    float rotationSpeed;
    float speed;
    int type;
    float attackCooldown;
    bool isDying;
    float deathTimer;
} Enemy3D;

typedef struct {
    Vector3 position;
    bool active;
    int type;
    float rotation;
    float lifetime;
} PowerUp3D;

typedef struct {
    Vector3 position;
    bool active;
    float rotation;
} AmmoPack;

static Bullet3D bullets[MAX_BULLETS];
static Enemy3D enemies[MAX_ENEMIES];
static PowerUp3D powerups[MAX_POWERUPS];
static AmmoPack ammoPacks[MAX_AMMO];
static int playerHealth = PLAYER_HEALTH;
static int playerMaxHealth = PLAYER_HEALTH;
static int playerAmmo = 30;
static int playerMaxAmmo = 30;
static int score = 0;
static int kills = 0;
static float gameTime = 0;
static float enemySpawnTimer = 0;
static float shootCooldown = 0;
static float powerupSpawnTimer = 0;
static bool gameOver = false;
static bool gamePaused = false;
static bool gameStarted = false;
static bool showIntro = true;
static float introTimer = 3.0f;
static float powerupSpeedBoost = 1.0f;
static float powerupDamageBoost = 1.0f;
static float powerupTimer = 0;
static int currentWave = 1;
static int enemiesInWave = 8;
static int waveKills = 0;
static bool waveComplete = false;
static float waveCompleteTimer = 3.0f;
static float waveTimer = 0;
static float nextWaveTime = 60.0f;

typedef enum {
    STATE_INTRO,
    STATE_MAIN_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAME_OVER
} GameState;

static GameState gameState = STATE_INTRO;
static int menuSelection = 0;
static float menuCooldown = 0.2f;

static float cameraYaw = 0.0f;
static float cameraPitch = 0.0f;
static Vector3 cameraPosition = {0};
static Vector3 cameraTarget = {0};


void Init3DGame() {
    printf("=== 3D SHOOTER INIT ===\n");
    
    srand(time(NULL));

    memset(bullets, 0, sizeof(bullets));
    memset(enemies, 0, sizeof(enemies));
    memset(powerups, 0, sizeof(powerups));
    memset(ammoPacks, 0, sizeof(ammoPacks));
    
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = false;
        bullets[i].size = 0.1f;
    }
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = false;
        enemies[i].isDying = false;
    }
    
    for (int i = 0; i < MAX_POWERUPS; i++) {
        powerups[i].active = false;
        powerups[i].lifetime = 15.0f;
    }
    
    for (int i = 0; i < MAX_AMMO; i++) {
        ammoPacks[i].active = false;
    }

    playerHealth = playerMaxHealth;
    playerAmmo = playerMaxAmmo;
    score = 0;
    kills = 0;
    gameTime = 0;
    gameOver = false;
    gamePaused = false;
    currentWave = 1;
    enemiesInWave = 8;
    waveKills = 0;
    waveComplete = false;
    waveTimer = 0;
    nextWaveTime = 60.0f;
    powerupSpeedBoost = 1.0f;
    powerupDamageBoost = 1.0f;
    powerupTimer = 0;

    cameraYaw = 0;
    cameraPitch = 0;
    
    printf("Game initialized! Wave: %d\n", currentWave);
}

void SpawnWave() {
    waveKills = 0;
    waveComplete = false;
    waveCompleteTimer = 3.0f;
    
    printf("=== WAVE %d ===\n", currentWave);
    printf("Enemies in wave: %d\n", enemiesInWave);
    
    int spawned = 0;
    for (int i = 0; i < enemiesInWave; i++) {
        for (int j = 0; j < MAX_ENEMIES; j++) {
            if (!enemies[j].active) {
                float angle = (rand() % 360) * 3.14159f / 180.0f;
                float distance = 10 + (rand() % (int)(ARENA_SIZE/2 - 15));
                
                enemies[j].position = (Vector3){
                    cosf(angle) * distance,
                    1.0f,
                    sinf(angle) * distance
                };
                
                enemies[j].active = true;
                enemies[j].isDying = false;
                enemies[j].velocity = (Vector3){0, 0, 0};
                enemies[j].deathTimer = 0;

                float waveMod = 1.0f + (currentWave - 1) * 0.3f;
                
 
                int typeChance = rand() % 100;
                if (currentWave >= 10 && typeChance < 15) {

                    enemies[j].type = 3;
                    enemies[j].health = (int)(200 * waveMod);
                    enemies[j].size = 2.5f;
                    enemies[j].speed = 0.8f;
                    enemies[j].color = (Color){200, 50, 50, 255};
                } else if (currentWave >= 6 && typeChance < 25) {

                    enemies[j].type = 2;
                    enemies[j].health = (int)(120 * waveMod);
                    enemies[j].size = 1.8f;
                    enemies[j].speed = 1.0f;
                    enemies[j].color = (Color){100, 50, 150, 255};
                } else if (currentWave >= 3 && typeChance < 40) {

                    enemies[j].type = 1;
                    enemies[j].health = (int)(40 * waveMod);
                    enemies[j].size = 0.7f;
                    enemies[j].speed = (ENEMY_SPEED_MAX + 2.0f) * waveMod;
                    enemies[j].color = (Color){50, 200, 100, 255};
                } else {

                    enemies[j].type = 0;
                    enemies[j].health = (int)(60 * waveMod);
                    enemies[j].size = 1.0f + (rand() % 100) / 100.0f;
                    enemies[j].speed = (ENEMY_SPEED_MIN + (rand() % 100) / 100.0f * (ENEMY_SPEED_MAX - ENEMY_SPEED_MIN)) * waveMod;
                    enemies[j].color = (Color){255, 50 + rand() % 100, 50 + rand() % 100, 255};
                }
                
                enemies[j].rotation = 0;
                enemies[j].rotationSpeed = (rand() % 100 - 50) / 50.0f;
                enemies[j].attackCooldown = 0;
                spawned++;
                break;
            }
        }
    }
    
    printf("Spawned %d enemies\n", spawned);
}

void UpdateCameraControl() {
    GameObject** playerObj = GetPlayerObject();
    if (!*playerObj) return;

    if (gameState != STATE_PLAYING) return;
    
    Vector2 mouseDelta = GetMouseDelta();
    
    cameraYaw += mouseDelta.x * MOUSE_SENSITIVITY;
    cameraPitch -= mouseDelta.y * MOUSE_SENSITIVITY;
    
    if (cameraPitch > 1.5f) cameraPitch = 1.5f;
    if (cameraPitch < -1.5f) cameraPitch = -1.5f;
    
    Vector3 direction;
    direction.x = cosf(cameraYaw) * cosf(cameraPitch);
    direction.y = sinf(cameraPitch);
    direction.z = sinf(cameraYaw) * cosf(cameraPitch);
    
    cameraPosition.x = (*playerObj)->position.x;
    cameraPosition.y = (*playerObj)->position.y + 1.6f;
    cameraPosition.z = (*playerObj)->position.z;
    
    cameraTarget.x = cameraPosition.x + direction.x;
    cameraTarget.y = cameraPosition.y + direction.y;
    cameraTarget.z = cameraPosition.z + direction.z;
    
    Camera3D* cam = GetCamera();
    cam->position = cameraPosition;
    cam->target = cameraTarget;
}

void UpdatePlayerMovement(float deltaTime) {
    GameObject** playerObj = GetPlayerObject();
    if (!*playerObj || gameState != STATE_PLAYING) return;

    if (!(*playerObj)->physics.isGrounded) {
        PlayerPhysicsSettings* settings = GetPlayerSettings();
        (*playerObj)->physics.velocity.y += settings->gravity * deltaTime;
        
        if ((*playerObj)->physics.velocity.y < -30.0f) {
            (*playerObj)->physics.velocity.y = -30.0f;
        }
    }
    
    float speed = PLAYER_SPEED * powerupSpeedBoost;
    if (IsKeyDown(KEY_LEFT_SHIFT)) {
        speed = PLAYER_RUN_SPEED * powerupSpeedBoost;
    }
    
    Vector3 move = {0, 0, 0};
    
    if (IsKeyDown(KEY_W)) move.z -= 1.0f;
    if (IsKeyDown(KEY_S)) move.z += 1.0f;
    if (IsKeyDown(KEY_A)) move.x -= 1.0f;
    if (IsKeyDown(KEY_D)) move.x += 1.0f;
    
    float length = sqrtf(move.x*move.x + move.z*move.z);
    if (length > 0) {
        move.x /= length;
        move.z /= length;
    }
    
    float sinYaw = sinf(cameraYaw);
    float cosYaw = cosf(cameraYaw);
    
    Vector3 rotatedMove;
    rotatedMove.x = move.x * cosYaw - move.z * sinYaw;
    rotatedMove.z = move.x * sinYaw + move.z * cosYaw;
    rotatedMove.y = 0;

    (*playerObj)->physics.velocity.x = rotatedMove.x * speed;
    (*playerObj)->physics.velocity.z = rotatedMove.z * speed;

    if (IsKeyPressed(KEY_SPACE) && (*playerObj)->physics.isGrounded) {
        (*playerObj)->physics.velocity.y = 12.0f;
        (*playerObj)->physics.isGrounded = false;
    }

    Vector3 newPos = (*playerObj)->position;
    newPos.x += (*playerObj)->physics.velocity.x * deltaTime;
    newPos.y += (*playerObj)->physics.velocity.y * deltaTime;
    newPos.z += (*playerObj)->physics.velocity.z * deltaTime;

    float arenaHalf = ARENA_SIZE / 2.0f;
    float playerRadius = (*playerObj)->size.x / 2.0f;

    if (newPos.x < -arenaHalf + playerRadius) {
        newPos.x = -arenaHalf + playerRadius;
        (*playerObj)->physics.velocity.x = 0;
    } else if (newPos.x > arenaHalf - playerRadius) {
        newPos.x = arenaHalf - playerRadius;
        (*playerObj)->physics.velocity.x = 0;
    }

    if (newPos.z < -arenaHalf + playerRadius) {
        newPos.z = -arenaHalf + playerRadius;
        (*playerObj)->physics.velocity.z = 0;
    } else if (newPos.z > arenaHalf - playerRadius) {
        newPos.z = arenaHalf - playerRadius;
        (*playerObj)->physics.velocity.z = 0;
    }

    float groundLevel = 0.5f;
    if (newPos.y < groundLevel) {
        newPos.y = groundLevel;
        (*playerObj)->physics.velocity.y = 0;
        (*playerObj)->physics.isGrounded = true;
    }
    
    (*playerObj)->position = newPos;

    if ((*playerObj)->physics.isGrounded) {
        (*playerObj)->physics.velocity.x *= 0.9f;
        (*playerObj)->physics.velocity.z *= 0.9f;
    }
}

void CreateBullet3D(Vector3 startPos, Vector3 direction) {
    if (playerAmmo <= 0) {
        PlaySoundEffect("no_ammo", "no_ammo.wav", 0.5f, 0);
        return;
    }
    
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].position = startPos;
            
            float spread = 0.02f;
            Vector3 spreadVec = {
                (rand() % 100 - 50) / 50.0f * spread,
                (rand() % 100 - 50) / 50.0f * spread,
                (rand() % 100 - 50) / 50.0f * spread
            };
            
            Vector3 finalDir = Vector3Normalize((Vector3){
                direction.x + spreadVec.x,
                direction.y + spreadVec.y,
                direction.z + spreadVec.z
            });
            
            bullets[i].velocity = (Vector3){
                finalDir.x * BULLET_SPEED,
                finalDir.y * BULLET_SPEED,
                finalDir.z * BULLET_SPEED
            };
            
            bullets[i].active = true;
            bullets[i].lifetime = 1.5f;
            bullets[i].size = 0.15f;
            bullets[i].color = (Color){255, 255, 100, 255};

            playerAmmo--;

            PlaySoundEffect("shoot", "shoot.wav", 0.3f * powerupDamageBoost, 0);
            CreateSparkEmitter(startPos, 3);
            break;
        }
    }
}

void CreatePowerUp(Vector3 position, int type) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (!powerups[i].active) {
            powerups[i].position = position;
            powerups[i].position.y = 1.0f;
            powerups[i].active = true;
            powerups[i].type = type;
            powerups[i].rotation = 0;
            powerups[i].lifetime = 15.0f;
            break;
        }
    }
}

void CreateAmmoPack(Vector3 position) {
    for (int i = 0; i < MAX_AMMO; i++) {
        if (!ammoPacks[i].active) {
            ammoPacks[i].position = position;
            ammoPacks[i].position.y = 1.0f;
            ammoPacks[i].active = true;
            ammoPacks[i].rotation = 0;
            break;
        }
    }
}

void UpdateGameLogic(float deltaTime) {
    if (menuCooldown > 0) {
        menuCooldown -= deltaTime;
    }
    
    switch (gameState) {
        case STATE_INTRO:
            introTimer -= deltaTime;
            if (introTimer <= 0 || IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                gameState = STATE_MAIN_MENU;
                menuSelection = 0;
                EnableCursor();
            }
            break;
            
        case STATE_MAIN_MENU:
            if (menuCooldown <= 0) {
                if (IsKeyPressed(KEY_UP)) {
                    menuSelection--;
                    if (menuSelection < 0) menuSelection = 1;
                    PlaySoundEffect("menu_move", "menu_move.wav", 0.3f, 0);
                    menuCooldown = 0.2f;
                }
                if (IsKeyPressed(KEY_DOWN)) {
                    menuSelection = (menuSelection + 1) % 2;
                    PlaySoundEffect("menu_move", "menu_move.wav", 0.3f, 0);
                    menuCooldown = 0.2f;
                }
                if (IsKeyPressed(KEY_ENTER)) {
                    PlaySoundEffect("menu_select", "menu_select.wav", 0.5f, 0);
                    
                    if (menuSelection == 0) {
                        gameState = STATE_PLAYING;
                        gameStarted = true;
                        Init3DGame();

                        GameObject** playerObj = GetPlayerObject();
                        if (*playerObj) {
                            (*playerObj)->position.y = 1.0f;
                            (*playerObj)->physics.isGrounded = true;
                            (*playerObj)->physics.velocity = (Vector3){0, 0, 0};
                        }
                        
                        SpawnWave();
                        DisableCursor();
                    } else if (menuSelection == 1) {
                        CloseEngine();
                        exit(0);
                    }
                    menuCooldown = 0.2f;
                }
            }
            if (IsKeyPressed(KEY_ESCAPE)) {
                CloseEngine();
                exit(0);
            }
            break;
            
        case STATE_PLAYING:
            if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
                gameState = STATE_PAUSED;
                menuSelection = 0;
                EnableCursor();
                PlaySoundEffect("pause", "pause.wav", 0.3f, 0);
                break;
            }
            
            gameTime += deltaTime;
            waveTimer += deltaTime;

            if (waveTimer >= nextWaveTime) {
                currentWave++;
                enemiesInWave = 8 + currentWave * 2;
                if (enemiesInWave > MAX_ENEMIES) enemiesInWave = MAX_ENEMIES;
                
                waveTimer = 0;
                nextWaveTime = 60.0f - currentWave * 2; 
                if (nextWaveTime < 30.0f) nextWaveTime = 30.0f;
                
                printf("=== WAVE %d AUTO-SPAWN ===\n", currentWave);
                printf("Next wave in: %.1f seconds\n", nextWaveTime);

                for (int i = 0; i < 5; i++) {
                    for (int j = 0; j < MAX_ENEMIES; j++) {
                        if (!enemies[j].active) {
                            float angle = (rand() % 360) * 3.14159f / 180.0f;
                            float distance = 15 + (rand() % (int)(ARENA_SIZE/2 - 20));
                            
                            enemies[j].position = (Vector3){
                                cosf(angle) * distance,
                                1.0f,
                                sinf(angle) * distance
                            };
                            
                            enemies[j].active = true;
                            enemies[j].isDying = false;

                            float waveMod = 1.0f + (currentWave - 1) * 0.3f;
                            int typeChance = rand() % 100;
                            
                            if (typeChance < 20 + currentWave) {
                                enemies[j].type = 2; 
                                enemies[j].health = (int)(120 * waveMod);
                                enemies[j].size = 1.8f;
                                enemies[j].speed = 1.0f;
                                enemies[j].color = (Color){100, 50, 150, 255};
                            } else if (typeChance < 40 + currentWave * 2) {
                                enemies[j].type = 1; 
                                enemies[j].health = (int)(40 * waveMod);
                                enemies[j].size = 0.7f;
                                enemies[j].speed = (ENEMY_SPEED_MAX + 2.0f) * waveMod;
                                enemies[j].color = (Color){50, 200, 100, 255};
                            } else {
                                enemies[j].type = 0;
                                enemies[j].health = (int)(60 * waveMod);
                                enemies[j].size = 1.0f;
                                enemies[j].speed = ENEMY_SPEED_MIN * waveMod;
                                enemies[j].color = (Color){255, 100, 100, 255};
                            }
                            
                            enemies[j].rotationSpeed = (rand() % 100 - 50) / 50.0f;
                            break;
                        }
                    }
                }
            }

            if (powerupTimer > 0) {
                powerupTimer -= deltaTime;
                if (powerupTimer <= 0) {
                    powerupSpeedBoost = 1.0f;
                    powerupDamageBoost = 1.0f;
                }
            }
            
            GameObject** playerObj = GetPlayerObject();
            if (!*playerObj) break;

            shootCooldown -= deltaTime;
            
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && shootCooldown <= 0) {
                Camera3D* cam = GetCamera();
                Vector3 bulletStart = cam->position;
                
                Vector3 forward = Vector3Normalize(Vector3Subtract(cam->target, cam->position));
                bulletStart.x += forward.x * 1.5f;
                bulletStart.y += forward.y * 1.5f;
                bulletStart.z += forward.z * 1.5f;
                
                CreateBullet3D(bulletStart, forward);
                shootCooldown = 0.1f / powerupDamageBoost;
            }

            if (IsKeyPressed(KEY_R)) {
                if (playerAmmo < playerMaxAmmo) {
                    playerAmmo = playerMaxAmmo;
                    PlaySoundEffect("reload", "reload.wav", 0.5f, 0);
                }
            }

            for (int i = 0; i < MAX_BULLETS; i++) {
                if (!bullets[i].active) continue;
                
                bullets[i].position.x += bullets[i].velocity.x * deltaTime;
                bullets[i].position.y += bullets[i].velocity.y * deltaTime;
                bullets[i].position.z += bullets[i].velocity.z * deltaTime;
                bullets[i].lifetime -= deltaTime;
                
                if (bullets[i].lifetime <= 0) {
                    bullets[i].active = false;
                    continue;
                }

                if (fabs(bullets[i].position.x) > ARENA_SIZE/2 || 
                    fabs(bullets[i].position.z) > ARENA_SIZE/2) {
                    bullets[i].active = false;
                    CreateSparkEmitter(bullets[i].position, 5);
                    continue;
                }

                for (int j = 0; j < MAX_ENEMIES; j++) {
                    if (!enemies[j].active || enemies[j].isDying) continue;
                    
                    float dx = bullets[i].position.x - enemies[j].position.x;
                    float dy = bullets[i].position.y - enemies[j].position.y;
                    float dz = bullets[i].position.z - enemies[j].position.z;
                    float distance = sqrtf(dx*dx + dy*dy + dz*dz);
                    
                    if (distance < enemies[j].size) {
                        int damage = 10 * powerupDamageBoost;
                        enemies[j].health -= damage;
                        bullets[i].active = false;
                        
                        CreateSparkEmitter(bullets[i].position, 8);
                        PlaySoundEffect("hit", "hit.wav", 0.3f, 0);

                        if (enemies[j].type >= 2) {
                            for (int k = 0; k < 5; k++) {
                                Vector3 bloodPos = bullets[i].position;
                                bloodPos.x += (rand() % 100 - 50) / 50.0f;
                                bloodPos.y += (rand() % 100) / 50.0f;
                                bloodPos.z += (rand() % 100 - 50) / 50.0f;
                                CreateSparkEmitter(bloodPos, 2);
                            }
                        }
                        
                        if (enemies[j].health <= 0) {
                            enemies[j].isDying = true;
                            enemies[j].deathTimer = 3.0f; 
                            
                            score += 10 * (enemies[j].type + 1) * currentWave;
                            kills++;
                            waveKills++;

                            int dropChance = rand() % 100;
                            if (dropChance < 50) {
                                int itemType = rand() % 100;
                                if (itemType < 30) {
                                    CreateAmmoPack(enemies[j].position);
                                } else if (itemType < 60) { 
                                    CreatePowerUp(enemies[j].position, 0);
                                } else if (itemType < 80) { 
                                    CreatePowerUp(enemies[j].position, 1);
                                } else { 
                                    CreatePowerUp(enemies[j].position, 2);
                                }
                            }
                            
                            PlaySoundEffect("enemy_death", "enemy_death.wav", 0.5f, 0);
                        }
                        break;
                    }
                }
                
                if (bullets[i].position.y < 0) {
                    bullets[i].active = false;
                    CreateSparkEmitter(bullets[i].position, 3);
                }
            }

            int activeEnemies = 0;
            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (!enemies[i].active) continue;
                
                if (enemies[i].isDying) {

                    enemies[i].deathTimer -= deltaTime;

                    if (rand() % 100 < 30) {
                        Vector3 particlePos = enemies[i].position;
                        particlePos.x += (rand() % 100 - 50) / 50.0f * enemies[i].size;
                        particlePos.y += (rand() % 100) / 100.0f * enemies[i].size;
                        particlePos.z += (rand() % 100 - 50) / 50.0f * enemies[i].size;
                        
                        Color particleColor = enemies[i].color;
                        particleColor.a = 150;

                        if (enemies[i].deathTimer < 2.0f) {
                            CreateSparkEmitter(particlePos, 3);
                        }
                        if (enemies[i].deathTimer < 1.0f) {
                            CreateFireEffect(particlePos, 0.3f);
                        }
                    }
                    
                    if (enemies[i].deathTimer <= 0) {
                        enemies[i].active = false;
                        CreateFireEffect(enemies[i].position, 0.5f + enemies[i].type * 0.2f);
                        CreateSparkEmitter(enemies[i].position, 15 + enemies[i].type * 5);
                    }
                    continue;
                }
                
                activeEnemies++;
                enemies[i].rotation += enemies[i].rotationSpeed * deltaTime;

                float arenaHalf = ARENA_SIZE / 2.0f - enemies[i].size;
                if (enemies[i].position.x < -arenaHalf) enemies[i].position.x = -arenaHalf;
                if (enemies[i].position.x > arenaHalf) enemies[i].position.x = arenaHalf;
                if (enemies[i].position.z < -arenaHalf) enemies[i].position.z = -arenaHalf;
                if (enemies[i].position.z > arenaHalf) enemies[i].position.z = arenaHalf;
                
                Vector3 playerPos = (*playerObj)->position;
                Vector3 dirToPlayer = {
                    playerPos.x - enemies[i].position.x,
                    0,
                    playerPos.z - enemies[i].position.z
                };
                
                float distanceToPlayer = sqrtf(dirToPlayer.x*dirToPlayer.x + dirToPlayer.z*dirToPlayer.z);
                
                if (distanceToPlayer > 0) {
                    dirToPlayer.x /= distanceToPlayer;
                    dirToPlayer.z /= distanceToPlayer;
                    
                    float moveSpeed = enemies[i].speed;
                    if (enemies[i].type == 1) moveSpeed *= 1.5f;
                    if (enemies[i].type == 2) moveSpeed *= 0.7f;
                    if (enemies[i].type == 3) moveSpeed *= 0.5f;
                    
                    enemies[i].position.x += dirToPlayer.x * moveSpeed * deltaTime;
                    enemies[i].position.z += dirToPlayer.z * moveSpeed * deltaTime;
                    enemies[i].position.y = 1.0f + sinf(gameTime * 3 + i) * 0.2f;
                }
                
                enemies[i].attackCooldown -= deltaTime;
                if (distanceToPlayer < 3.0f && enemies[i].attackCooldown <= 0) {
                    int damage = 5 + enemies[i].type * 5;
                    playerHealth -= damage;
                    enemies[i].attackCooldown = 1.5f;
                    
                    enemies[i].position.x -= dirToPlayer.x * 1.5f;
                    enemies[i].position.z -= dirToPlayer.z * 1.5f;
                    
                    CreateFireEffect((*playerObj)->position, 0.2f);
                    PlaySoundEffect("hurt", "hurt.wav", 0.4f, 0);
                    
                    if (playerHealth <= 0) {
                        playerHealth = 0;
                        gameState = STATE_GAME_OVER;
                        menuSelection = 0;
                        EnableCursor();
                        PlaySoundEffect("game_over", "game_over.wav", 0.6f, 0);
                    }
                }
            }

            for (int i = 0; i < MAX_POWERUPS; i++) {
                if (!powerups[i].active) continue;
                
                powerups[i].rotation += deltaTime * 2.0f;
                powerups[i].lifetime -= deltaTime;
                
                if (powerups[i].lifetime <= 0) {
                    powerups[i].active = false;
                    continue;
                }
                
                Vector3 playerPos = (*playerObj)->position;
                float dx = playerPos.x - powerups[i].position.x;
                float dy = playerPos.y - powerups[i].position.y;
                float dz = playerPos.z - powerups[i].position.z;
                float distance = sqrtf(dx*dx + dy*dy + dz*dz);
                
                if (distance < 2.0f) {
                    powerups[i].active = false;
                    
                    switch (powerups[i].type) {
                        case 0:
                            playerHealth += 40;
                            if (playerHealth > playerMaxHealth) playerHealth = playerMaxHealth;
                            PlaySoundEffect("health", "health.wav", 0.5f, 0);
                            break;
                        case 1:
                            powerupSpeedBoost = 2.0f;
                            powerupTimer = 15.0f;
                            PlaySoundEffect("speed", "speed.wav", 0.5f, 0);
                            break;
                        case 2:
                            powerupDamageBoost = 2.0f;
                            powerupTimer = 15.0f;
                            PlaySoundEffect("damage", "damage.wav", 0.5f, 0);
                            break;
                    }
                    
                    CreateSparkEmitter(powerups[i].position, 15);
                }
            }

            for (int i = 0; i < MAX_AMMO; i++) {
                if (!ammoPacks[i].active) continue;
                
                ammoPacks[i].rotation += deltaTime * 3.0f;
                
                Vector3 playerPos = (*playerObj)->position;
                float dx = playerPos.x - ammoPacks[i].position.x;
                float dy = playerPos.y - ammoPacks[i].position.y;
                float dz = playerPos.z - ammoPacks[i].position.z;
                float distance = sqrtf(dx*dx + dy*dy + dz*dz);
                
                if (distance < 2.0f) {
                    ammoPacks[i].active = false;
                    playerAmmo += 15;
                    if (playerAmmo > playerMaxAmmo) playerAmmo = playerMaxAmmo;
                    PlaySoundEffect("ammo", "ammo.wav", 0.5f, 0);
                    CreateSparkEmitter(ammoPacks[i].position, 20);
                }
            }

            if (waveComplete) {
                waveCompleteTimer -= deltaTime;
                if (waveCompleteTimer <= 0) {
                    currentWave++;
                    enemiesInWave = 8 + currentWave * 2;
                    if (enemiesInWave > MAX_ENEMIES) enemiesInWave = MAX_ENEMIES;
                    
                    waveTimer = 0;
                    nextWaveTime = 60.0f - currentWave * 2;
                    if (nextWaveTime < 30.0f) nextWaveTime = 30.0f;
                    
                    SpawnWave();
                }
            } else if (activeEnemies == 0 && !waveComplete) {
                waveComplete = true;
                score += 100 * currentWave;
                PlaySoundEffect("wave_complete", "wave_complete.wav", 0.6f, 0);
            }

            enemySpawnTimer += deltaTime;
            if (enemySpawnTimer >= 5.0f && activeEnemies < enemiesInWave / 2) {
                for (int i = 0; i < 2; i++) {
                    for (int j = 0; j < MAX_ENEMIES; j++) {
                        if (!enemies[j].active) {
                            float angle = (rand() % 360) * 3.14159f / 180.0f;
                            float distance = 20 + (rand() % (int)(ARENA_SIZE/2 - 25));
                            
                            enemies[j].position = (Vector3){
                                cosf(angle) * distance,
                                1.0f,
                                sinf(angle) * distance
                            };
                            
                            enemies[j].active = true;
                            enemies[j].type = rand() % (currentWave >= 6 ? 3 : (currentWave >= 3 ? 2 : 1));
                            float waveMod = 1.0f + (currentWave - 1) * 0.2f;
                            enemies[j].health = (20 + currentWave * 8) * waveMod;
                            enemies[j].size = 0.8f + enemies[j].type * 0.3f;
                            enemies[j].speed = (ENEMY_SPEED_MIN + (ENEMY_SPEED_MAX - ENEMY_SPEED_MIN) * (rand() % 100) / 100.0f) * waveMod;
                            enemies[j].rotationSpeed = (rand() % 100 - 50) / 50.0f;
                            
                            switch (enemies[j].type) {
                                case 0: enemies[j].color = (Color){255, 100, 100, 255}; break;
                                case 1: enemies[j].color = (Color){100, 255, 100, 255}; break;
                                case 2: enemies[j].color = (Color){100, 100, 255, 255}; break;
                            }
                            break;
                        }
                    }
                }
                enemySpawnTimer = 0;
            }

            if (IsKeyPressed(KEY_H)) {
                playerHealth = playerMaxHealth;
                playerAmmo = playerMaxAmmo;
                PlaySoundEffect("heal", "heal.wav", 0.3f, 0);
            }
            break;
            
        case STATE_PAUSED:
            if (menuCooldown <= 0) {
                if (IsKeyPressed(KEY_UP)) {
                    menuSelection--;
                    if (menuSelection < 0) menuSelection = 2;
                    PlaySoundEffect("menu_move", "menu_move.wav", 0.3f, 0);
                    menuCooldown = 0.2f;
                }
                if (IsKeyPressed(KEY_DOWN)) {
                    menuSelection = (menuSelection + 1) % 3;
                    PlaySoundEffect("menu_move", "menu_move.wav", 0.3f, 0);
                    menuCooldown = 0.2f;
                }
                if (IsKeyPressed(KEY_ENTER)) {
                    PlaySoundEffect("menu_select", "menu_select.wav", 0.5f, 0);
                    
                    if (menuSelection == 0) {

                        gameState = STATE_PLAYING;
                        DisableCursor();
                    } else if (menuSelection == 1) {

                        gameState = STATE_PLAYING;
                        Init3DGame();
                        
                        GameObject** playerObj = GetPlayerObject();
                        if (*playerObj) {
                            (*playerObj)->position = (Vector3){0, 1.0f, 0};
                            (*playerObj)->physics.isGrounded = true;
                            (*playerObj)->physics.velocity = (Vector3){0, 0, 0};
                        }
                        
                        SpawnWave();
                        DisableCursor();
                    } else if (menuSelection == 2) {

                        gameState = STATE_MAIN_MENU;
                        menuSelection = 0;
                    }
                    menuCooldown = 0.2f;
                }
                if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
                    gameState = STATE_PLAYING;
                    DisableCursor();
                    PlaySoundEffect("unpause", "unpause.wav", 0.3f, 0);
                    menuCooldown = 0.2f;
                }
            }
            break;
            
        case STATE_GAME_OVER:
            if (menuCooldown <= 0) {
                if (IsKeyPressed(KEY_UP)) {
                    menuSelection--;
                    if (menuSelection < 0) menuSelection = 2;
                    PlaySoundEffect("menu_move", "menu_move.wav", 0.3f, 0);
                    menuCooldown = 0.2f;
                }
                if (IsKeyPressed(KEY_DOWN)) {
                    menuSelection = (menuSelection + 1) % 3;
                    PlaySoundEffect("menu_move", "menu_move.wav", 0.3f, 0);
                    menuCooldown = 0.2f;
                }
                if (IsKeyPressed(KEY_ENTER)) {
                    PlaySoundEffect("menu_select", "menu_select.wav", 0.5f, 0);
                    
                    if (menuSelection == 0) {

                        gameState = STATE_PLAYING;
                        Init3DGame();
                        
                        GameObject** playerObj = GetPlayerObject();
                        if (*playerObj) {
                            (*playerObj)->position = (Vector3){0, 1.0f, 0};
                            (*playerObj)->physics.isGrounded = true;
                            (*playerObj)->physics.velocity = (Vector3){0, 0, 0};
                        }
                        
                        SpawnWave();
                        DisableCursor();
                    } else if (menuSelection == 1) {

                        gameState = STATE_MAIN_MENU;
                        menuSelection = 0;
                    } else if (menuSelection == 2) {

                        CloseEngine();
                        exit(0);
                    }
                    menuCooldown = 0.2f;
                }
                if (IsKeyPressed(KEY_ESCAPE)) {
                    gameState = STATE_MAIN_MENU;
                    menuSelection = 0;
                    menuCooldown = 0.2f;
                }
            }
            break;
    }
}

void Draw3DGame() {

    float arenaHalf = ARENA_SIZE / 2.0f;

    DrawCube((Vector3){0, ARENA_WALL_HEIGHT/2, -arenaHalf}, 
             ARENA_SIZE, ARENA_WALL_HEIGHT, 2.0f, 
             (Color){100, 100, 100, 255});
    DrawCube((Vector3){0, ARENA_WALL_HEIGHT/2, arenaHalf}, 
             ARENA_SIZE, ARENA_WALL_HEIGHT, 2.0f, 
             (Color){100, 100, 100, 255});

    DrawCube((Vector3){-arenaHalf, ARENA_WALL_HEIGHT/2, 0}, 
             2.0f, ARENA_WALL_HEIGHT, ARENA_SIZE, 
             (Color){100, 100, 100, 255});

    DrawCube((Vector3){arenaHalf, ARENA_WALL_HEIGHT/2, 0}, 
             2.0f, ARENA_WALL_HEIGHT, ARENA_SIZE, 
             (Color){100, 100, 100, 255});

    for (int x = -4; x <= 4; x++) {
        for (int z = -4; z <= 4; z++) {
            float tileSize = ARENA_SIZE / 8.0f;
            Color tileColor = ((x + z) % 2 == 0) ? 
                (Color){80, 80, 80, 255} : (Color){60, 60, 60, 255};
            DrawCube((Vector3){x * tileSize, 0.01f, z * tileSize}, 
                     tileSize * 0.9f, 0.02f, tileSize * 0.9f, tileColor);
        }
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;
        
        if (enemies[i].isDying) {
            float pulse = sinf(gameTime * 10) * 0.3f + 0.7f;
            Color deathColor = enemies[i].color;
            deathColor.a = (unsigned char)(pulse * 200);
            
            DrawSphere(enemies[i].position, enemies[i].size * pulse, deathColor);

            for (int j = 0; j < 3; j++) {
                Vector3 particlePos = enemies[i].position;
                particlePos.x += (sinf(gameTime * 5 + j) * enemies[i].size * 1.5f);
                particlePos.y += (cosf(gameTime * 3 + j) * enemies[i].size * 1.5f);
                particlePos.z += (sinf(gameTime * 7 + j) * enemies[i].size * 1.5f);
                DrawSphere(particlePos, enemies[i].size * 0.3f * pulse, 
                          (Color){deathColor.r, deathColor.g, deathColor.b, (unsigned char)(pulse * 150)});
            }
            continue;
        }
        
        DrawSphere(enemies[i].position, enemies[i].size, enemies[i].color);

        if (enemies[i].type == 1) {
            for (int j = 0; j < 2; j++) {
                float angle = j * 3.14159f + enemies[i].rotation;
                Vector3 hornPos = {
                    enemies[i].position.x + cosf(angle) * enemies[i].size * 0.8f,
                    enemies[i].position.y + enemies[i].size * 0.8f,
                    enemies[i].position.z + sinf(angle) * enemies[i].size * 0.8f
                };
                DrawCylinder(hornPos, 0.05f, 0, enemies[i].size * 0.4f, 6, 
                            (Color){enemies[i].color.r/2, enemies[i].color.g/2, enemies[i].color.b/2, 255});
            }
        } else if (enemies[i].type == 2) {
            DrawSphereWires(enemies[i].position, enemies[i].size * 1.2f, 12, 12, 
                           (Color){50, 50, 50, 200});

            for (int j = 0; j < 4; j++) {
                float angle = j * 1.5708f + enemies[i].rotation;
                Vector3 gunPos = {
                    enemies[i].position.x + cosf(angle) * enemies[i].size * 0.8f,
                    enemies[i].position.y,
                    enemies[i].position.z + sinf(angle) * enemies[i].size * 0.8f
                };
                DrawCylinder(gunPos, 0.15f, 0.15f, enemies[i].size * 0.6f, 8, DARKGRAY);
            }
        } else if (enemies[i].type == 3) { 
            for (int j = 0; j < 8; j++) {
                float angle = (j / 8.0f) * 3.14159f * 2;
                Vector3 spikePos = {
                    enemies[i].position.x + cosf(angle) * enemies[i].size * 1.2f,
                    enemies[i].position.y + enemies[i].size * 1.5f,
                    enemies[i].position.z + sinf(angle) * enemies[i].size * 1.2f
                };
                DrawCylinder(spikePos, 0.2f, 0, enemies[i].size * 0.8f, 6, GOLD);
            }
        }

        Vector3 eyeOffset = {enemies[i].size * 0.3f, enemies[i].size * 0.2f, enemies[i].size * 0.7f};
        DrawSphere((Vector3){enemies[i].position.x - eyeOffset.x, 
                            enemies[i].position.y + eyeOffset.y, 
                            enemies[i].position.z + eyeOffset.z}, 
                  enemies[i].size * 0.15f, BLACK);
        DrawSphere((Vector3){enemies[i].position.x + eyeOffset.x, 
                            enemies[i].position.y + eyeOffset.y, 
                            enemies[i].position.z + eyeOffset.z}, 
                  enemies[i].size * 0.15f, BLACK);

        if (enemies[i].health < 100) {
            Vector3 healthBarPos = {
                enemies[i].position.x,
                enemies[i].position.y + enemies[i].size + 0.5f,
                enemies[i].position.z
            };
            
            float healthPercent = (float)enemies[i].health / 100.0f;
            DrawCube(healthBarPos, 1.5f, 0.1f, 0.1f, RED);
            DrawCube((Vector3){healthBarPos.x - 0.75f + healthPercent * 0.75f, 
                              healthBarPos.y, healthBarPos.z}, 
                    healthPercent * 1.5f, 0.08f, 0.08f, GREEN);
        }
    }

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) continue;
        
        DrawSphere(bullets[i].position, bullets[i].size, bullets[i].color);
        
        Vector3 trailStart = {
            bullets[i].position.x - bullets[i].velocity.x * 0.01f,
            bullets[i].position.y - bullets[i].velocity.y * 0.01f,
            bullets[i].position.z - bullets[i].velocity.z * 0.01f
        };
        DrawLine3D(trailStart, bullets[i].position, YELLOW);
    }

    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (!powerups[i].active) continue;
        
        Color powerupColor;
        const char* symbol = "";
        switch (powerups[i].type) {
            case 0: 
                powerupColor = RED; 
                symbol = "H";
                break;
            case 1: 
                powerupColor = GREEN; 
                symbol = "S";
                break;
            case 2: 
                powerupColor = BLUE; 
                symbol = "D";
                break;
        }
        
        Vector3 drawPos = powerups[i].position;
        drawPos.y += sinf(gameTime * 3 + i) * 0.3f;
 
        float pulse = sinf(gameTime * 5) * 0.2f + 0.8f;
        DrawSphere(drawPos, 0.5f * pulse, (Color){powerupColor.r, powerupColor.g, powerupColor.b, 150});

        DrawCubeWires(powerups[i].position, 0.6f, 0.6f, 0.6f, powerupColor);
    }

    for (int i = 0; i < MAX_AMMO; i++) {
        if (!ammoPacks[i].active) continue;
        
        Vector3 drawPos = ammoPacks[i].position;
        drawPos.y += sinf(gameTime * 4 + i) * 0.4f;

        float pulse = sinf(gameTime * 6) * 0.3f + 0.7f;
        DrawCube(drawPos, 0.4f, 0.4f, 0.4f, (Color){255, 255, 0, (unsigned char)(pulse * 200)});
        DrawCubeWires(drawPos, 0.5f, 0.5f, 0.5f, YELLOW);
    }

    if (gameState == STATE_PLAYING) {
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();
 
        DrawLine(screenWidth/2 - 15, screenHeight/2, 
                 screenWidth/2 + 15, screenHeight/2, RED);
        DrawLine(screenWidth/2, screenHeight/2 - 15, 
                 screenWidth/2, screenHeight/2 + 15, RED);

        DrawCircle(screenWidth/2, screenHeight/2, 2, RED);

        Color circleColor = (playerHealth < 30) ? RED : (Color){255, 0, 0, 100};
        DrawCircleLines(screenWidth/2, screenHeight/2, 8, circleColor);
    }
}

void DrawGameUI() {
    if (gameState != STATE_PLAYING) return;
    
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    DrawRectangle(0, 0, screenWidth, 80, (Color){0, 0, 0, 200});

    DrawText("HEALTH", 20, 10, 24, RED);
    DrawRectangle(20, 40, 200, 20, (Color){50, 0, 0, 255});
    float healthPercent = (float)playerHealth / playerMaxHealth;
    DrawRectangle(20, 40, (int)(200 * healthPercent), 20, RED);
    DrawText(TextFormat("%d/%d", playerHealth, playerMaxHealth), 230, 40, 20, WHITE);

    DrawText("AMMO", 300, 10, 24, YELLOW);
    DrawRectangle(300, 40, 150, 20, (Color){50, 50, 0, 255});
    float ammoPercent = (float)playerAmmo / playerMaxAmmo;
    DrawRectangle(300, 40, (int)(150 * ammoPercent), 20, YELLOW);
    DrawText(TextFormat("%d/%d", playerAmmo, playerMaxAmmo), 460, 40, 20, WHITE);
    DrawText("[R]", 520, 40, 20, LIGHTGRAY);

    DrawText(TextFormat("WAVE: %d", currentWave), screenWidth/2 - 100, 10, 28, ORANGE);
    DrawText(TextFormat("NEXT: %.0fs", nextWaveTime - waveTimer), screenWidth/2 - 100, 40, 20, LIGHTGRAY);

    DrawText(TextFormat("SCORE: %d", score), screenWidth - 250, 10, 28, YELLOW);
 
    DrawText(TextFormat("KILLS: %d", kills), screenWidth - 250, 40, 20, WHITE);

    int enemiesLeft = 0;
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active && !enemies[i].isDying) enemiesLeft++;
    }
    DrawText(TextFormat("ENEMIES: %d", enemiesLeft), 
            screenWidth - 120, 65, 18, enemiesLeft > 10 ? RED : WHITE);
 
    if (powerupTimer > 0) {
        int boostY = 90;
        if (powerupSpeedBoost > 1.0f) {
            DrawRectangle(20, boostY, 180, 25, (Color){0, 100, 0, 150});
            DrawText(TextFormat("SPEED BOOST: %.1f", powerupTimer), 25, boostY + 3, 20, GREEN);
            boostY += 30;
        }
        if (powerupDamageBoost > 1.0f) {
            DrawRectangle(20, boostY, 180, 25, (Color){100, 0, 0, 150});
            DrawText(TextFormat("DAMAGE BOOST: %.1f", powerupTimer), 25, boostY + 3, 20, RED);
        }
    }

    if (waveComplete && waveCompleteTimer > 0) {
        const char* waveText = TextFormat("WAVE %d COMPLETE!", currentWave);
        int textSize = 50;
        int textWidth = MeasureText(waveText, textSize);
        
        float pulse = sinf(gameTime * 10) * 0.5f + 0.5f;
        DrawText(waveText, screenWidth/2 - textWidth/2, screenHeight/2 - 100, textSize, 
                (Color){255, (unsigned char)(255 * pulse), (unsigned char)(100 * pulse), 255});
        
        DrawText("PREPARE FOR NEXT WAVE!", screenWidth/2 - 150, screenHeight/2 - 40, 24, YELLOW);
    }
 
    DrawRectangle(0, screenHeight - 50, screenWidth, 50, (Color){0, 0, 0, 200});

    const char* controls = "WASD:Move | MOUSE:Aim | LMB:Shoot | SPACE:Jump | SHIFT:Run | R:Reload | P:Pause | H:Heal(Cheat)";
    DrawText(controls, 10, screenHeight - 40, 18, LIGHTGRAY);

    DrawText(TextFormat("TIME: %.1f", gameTime), screenWidth - 150, screenHeight - 40, 20, WHITE);

    if (playerHealth < 30) {
        float blink = sinf(gameTime * 10) * 0.5f + 0.5f;
        DrawText("LOW HEALTH!", screenWidth/2 - 80, screenHeight/2 + 100, 30, 
                (Color){255, (unsigned char)(50 * blink), (unsigned char)(50 * blink), 255});
    }

    if (playerAmmo < 5) {
        float blink = sinf(gameTime * 8) * 0.5f + 0.5f;
        DrawText("LOW AMMO!", screenWidth/2 - 70, screenHeight/2 + 140, 30, 
                (Color){255, 255, (unsigned char)(50 * blink), 255});
    }

    DrawFPS(screenWidth - 100, 90);
}

int main(void) {
    InitEngine(1280, 720, "ARENA SHOOTER - QWEE Engine", false);
    
    PlayerPhysicsSettings* settings = GetPlayerSettings();
    if (settings) {
        settings->walkSpeed = PLAYER_SPEED;
        settings->runSpeed = PLAYER_RUN_SPEED;
        settings->jumpForce = 12.0f;
        settings->gravity = -25.0f;
        settings->playerHeight = 1.8f;
        settings->playerRadius = 0.5f;
        settings->airControl = 0.3f;
        settings->groundFriction = 0.9f;
    }

    GameObject* player = CreatePlayer("Player", 0, 1.0f, 0, true, true);
    if (player) {
        player->physics.isGrounded = true;
        player->physics.velocity = (Vector3){0, 0, 0};
    }

    srand(time(NULL));
    for (int i = 0; i < 15; i++) {
        char name[32];
        snprintf(name, sizeof(name), "Obstacle%d", i);
        
        float x = (rand() % (int)(ARENA_SIZE - 10)) - (ARENA_SIZE/2 - 5);
        float z = (rand() % (int)(ARENA_SIZE - 10)) - (ARENA_SIZE/2 - 5);

        if (fabs(x) > 5 || fabs(z) > 5) {
            CreateCube(name, x, 2, z, false, true, NULL, 
                      (Color){100, 80, 60, 255});
            
            GameObject* wall = FindObject(name);
            if (wall) {
                wall->size.x = 2 + (rand() % 100) / 50.0f;
                wall->size.y = 3 + (rand() % 100) / 50.0f;
                wall->size.z = 2 + (rand() % 100) / 50.0f;
                wall->isStatic = true;
            }
        }
    }
    
    Camera3D* cam = GetCamera();
    cam->fovy = 90.0f;
    
    if (FileExists("music.mp3")) {
        LoadAndPlayMusic("GameMusic", "music.mp3", 0.3f, true);
    }
    
    printf("\n=== ARENA SHOOTER ===\n");
    printf("Features:\n");
    printf("  - Limited arena with walls\n");
    printf("  - Limited ammo system\n");
    printf("  - Enemy death particles (3 seconds)\n");
    printf("  - High drop chance (50%%)\n");
    printf("  - Auto-increasing waves\n");
    printf("  - Progressive enemy difficulty\n");
    printf("  - Ammo packs\n");
    
    while (!WindowShouldClose()) {
        float dt = GetDeltaTime();
        if (dt > 0.1f) dt = 0.1f;
        
        if (gameState == STATE_PLAYING) {
            UpdateCameraControl();
            UpdatePlayerMovement(dt);
        }
        
        UpdateGameLogic(dt);
        UpdateEngine(dt);
        
        BeginDrawing();
        
        switch (gameState) {
            case STATE_INTRO:

                ClearBackground((Color){10, 10, 30, 255});
                DrawText("ARENA SHOOTER", GetScreenWidth()/2 - 150, GetScreenHeight()/2 - 50, 50, RED);
                DrawText("QWEE Engine", GetScreenWidth()/2 - 100, GetScreenHeight()/2 + 20, 30, BLUE);
                DrawText("Press ENTER", GetScreenWidth()/2 - 100, GetScreenHeight()/2 + 80, 24, GREEN);
                break;
                
            case STATE_MAIN_MENU:

                ClearBackground((Color){20, 20, 40, 255});
                DrawText("ARENA SHOOTER", GetScreenWidth()/2 - 200, 100, 60, RED);
                
                const char* options[] = {"[ PLAY GAME ]", "[ EXIT ]"};
                for (int i = 0; i < 2; i++) {
                    int yPos = GetScreenHeight()/2 + i * 60;
                    if (i == menuSelection) {
                        DrawText(options[i], GetScreenWidth()/2 - 100, yPos, 40, GREEN);
                    } else {
                        DrawText(options[i], GetScreenWidth()/2 - 100, yPos, 40, WHITE);
                    }
                }
                break;
                
            case STATE_PLAYING:
                ClearBackground((Color){30, 30, 50, 255});
                
                BeginMode3D(*GetCamera());
                
                GameObject** objects = GetObjects();
                int* objectCount = GetObjectCount();
                
                for (int i = 0; i < *objectCount; i++) {
                    if (objects[i] && objects[i]->isActive) {
                        DrawObject(objects[i]);
                    }
                }
                
                Draw3DGame();
                
                EndMode3D();
                
                DrawGameUI();
                break;
                
            case STATE_PAUSED:
                ClearBackground((Color){30, 30, 50, 255});
                BeginMode3D(*GetCamera());
                EndMode3D();
                
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){0, 0, 0, 180});
                DrawText("PAUSED", GetScreenWidth()/2 - 100, GetScreenHeight()/2 - 50, 50, YELLOW);
                break;
                
            case STATE_GAME_OVER:
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){0, 0, 0, 200});
                DrawText("GAME OVER", GetScreenWidth()/2 - 150, 100, 60, RED);
                DrawText(TextFormat("Final Wave: %d", currentWave), GetScreenWidth()/2 - 100, 200, 30, WHITE);
                DrawText(TextFormat("Score: %d", score), GetScreenWidth()/2 - 100, 240, 30, WHITE);
                break;
        }
        
        if (gameState != STATE_INTRO) {
            DrawFPS(10, 10);
        }
        
        EndDrawing();
    }
    
    CloseEngine();
    printf("\n=== FINAL STATS ===\n");
    printf("Wave: %d\n", currentWave);
    printf("Score: %d\n", score);
    printf("Kills: %d\n", kills);
    printf("Time: %.1f seconds\n", gameTime);
    
    return 0;
}