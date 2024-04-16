#include <iostream>
#include <deque>
#include <raylib.h>
#include <raymath.h>

using namespace std;

Color black = {0, 0, 0, 255};
Color blue = { 102, 191, 255, 255};
Color green = {0, 228, 48, 255};
Color red = {255, 0, 0, 255};
Color pink = {255, 109, 194, 255};
int cellSize = 25;
int cellCount = 25;
int offset = 30;
double lastUpdateTime = 0;
double initialUpdateInterval = 0.15;
double updateInterval = initialUpdateInterval;
double speedIncreaseFactor = 0.025;

bool ElementInDeque(Vector2 element, deque<Vector2> deque)
{
    for (unsigned int i = 0; i < deque.size(); i++)
    {
        if (Vector2Equals(deque[i], element))
        {
            return true;
        }
    }
    return false;
}

bool eventTriggered(double interval)
{
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval)
    {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

class Snake
{
    public:
        deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
        Vector2 direction = {1,0};
        bool addSegment=false;

    void Draw()
    {
        for (unsigned int i = 0; i < body.size(); i++)
        {
            for (unsigned int i=0; i<body.size(); i++) 
            {
                float x = body[i].x;
                float y = body[i].y;
                Rectangle segment = Rectangle{x*cellSize, y*cellSize, (float)cellSize, (float)cellSize};
                DrawRectangleRounded(segment, 0.5, 6, green);
            }
        }
    }

    void Update()
    {
        body.push_front(Vector2Add(body[0], direction));
        if (addSegment == true)
        {
            addSegment = false;
        }
        else
        {
            body.push_front(Vector2Add(body[0],direction));
            if(addSegment == true)
            {
                addSegment = false;
            }
            else
            {
                body.pop_back();   
            }
        }
    }

    void Reset()
    {
        body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
        direction = {1, 0};
    }
};

class Food
{
public:
    Vector2 position = {5, 6};
    Texture2D texture;

    Food(deque<Vector2> snakeBody)
    {
        Image image = LoadImage("apple.png");
        ImageResizeNN(&image, cellSize, cellSize);
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = GenerateRandomPos(snakeBody);
    }

    ~Food()
    {
        UnloadTexture(texture);
    }

    void Draw()
    {
        DrawTexture(texture, offset + position.x * cellSize, offset + position.y * cellSize, WHITE);
    }

    Vector2 GenerateRandomCell()
    {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return Vector2{x, y};
    }

    Vector2 GenerateRandomPos(deque<Vector2> snakeBody)
    {
        Vector2 position = GenerateRandomCell();
        while (ElementInDeque(position, snakeBody))
        {
            position = GenerateRandomCell();
        }
        return position;
    }
};

class Game
{
public:
    Snake snake = Snake();
    Food food = Food(snake.body);
    bool running = true;
    int score = 0;

    void Draw()
    {
        food.Draw();
        snake.Draw();
    }

    void Update()
    {
        if (running)
        {
            snake.Update();
            CheckEatFood();
            CheckCollisionWithEdges();
            CheckCollisionWithTail();
        }
    }

    void CheckEatFood()
    {
        if (Vector2Equals(snake.body[0], food.position))
        {
            food.position = food.GenerateRandomPos(snake.body);
            snake.addSegment = true;
            score++;
            updateInterval -= speedIncreaseFactor * initialUpdateInterval;
            if (updateInterval < 0.05)
            {
                food.position = food.GenerateRandomPos(snake.body);
                snake.addSegment = true;
            }
        }
    }

    void CheckCollisionWithEdges()
    {
        if (snake.body[0].x == cellCount) snake.body[0].x = 0;
        else if (snake.body[0].x == -1) snake.body[0].x = cellCount - 1;
        if (snake.body[0].y == cellCount) snake.body[0].y = 0;
        else if (snake.body[0].y == -1) snake.body[0].y = cellCount - 1;
    }

    /* if it touch the edge, reset the game.
    void CheckCollisionWithEdges()
    {
        if (snake.body[0].x == cellCount || snake.body[0].x == -1)
        {
            GameOver();
        }
        if (snake.body[0].y == cellCount || snake.body[0].y == -1)
        {
            GameOver();
        }
    }
    */

    void CheckCollisionWithTail()
    {
        deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if (ElementInDeque(snake.body[0], headlessBody)) GameOver();
    }

    void GameOver()
    {
        snake.Reset();
        food.position = food.GenerateRandomPos(snake.body);
        running = false;
        score = 0;
        updateInterval = initialUpdateInterval;
    }
};

int main()
{
    cout << "START GAME" << endl;
    InitWindow(2 * offset + cellSize * cellCount, 2 * offset + cellSize * cellCount, "Ouroboros");
    SetTargetFPS(100);

    Game game = Game();

    while (WindowShouldClose() == false)
    {
        BeginDrawing();

        if (eventTriggered(updateInterval))
        {
            game.Update();
        }
        if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1)
        {
            game.snake.direction = {0, -1};
            game.running = true;
        }
        if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1)
        {
            game.snake.direction = {0, 1};
            game.running = true;
        }
        if(IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1)
        {
            game.snake.direction = {-1, 0};
            game.running = true;
        }
        if(IsKeyPressed(KEY_RIGHT) && game.snake.direction.x !=-1)
        {
            game.snake.direction = {1, 0};
            game.running = true;
        }

        ClearBackground(black);
        DrawRectangleLinesEx(Rectangle{(float)offset - 5, (float)offset - 5, (float)cellSize * cellCount + 10, (float)cellSize * cellCount + 10}, 5, red);
        DrawText("Ouroboros", offset - 5, 5, 20, red);
        DrawText("Score: ", offset - 5, offset + cellSize * cellCount + 5, 20, red);
        DrawText(TextFormat("%i", game.score), offset + 70, offset + cellSize * cellCount + 5, 20, red);
        game.Draw();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
