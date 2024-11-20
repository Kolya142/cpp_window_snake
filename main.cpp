#include <SFML/Graphics.hpp>
#include <unistd.h>
#include <math.h>
#include <string>
#include <cstring>
#include <sstream>
// #define NOTCODE

std::vector<std::string> Split(const std::string& str, char separator) {
    std::vector<std::string> result;
    std::string token;
    std::istringstream tokenStream(str);

    while (std::getline(tokenStream, token, separator)) {
        if (token[0] == '/') {
            continue;
        }
        result.push_back(token);
    }

    return result;
}

struct Point {
    int x;
    int y;
    inline Point(int _x, int _y) {x=_x;y=_y;}
};

struct PointF {
    float x;
    float y;
    inline PointF(float _x, float _y) {x=_x;y=_y;}
};
float easeOutExpo(float x) { // https://easings.net/
    return x == 1 ? 1 : 1 - powf(2, -10 * x);
}
int main()
{
    bool glow = 1;
    sf::RenderWindow window(sf::VideoMode(300, 300), "Window snake");
    sf::VideoMode screen = sf::VideoMode::getDesktopMode();
    int screenw = screen.width;int hsw = screenw / 2;
    int screenh = screen.height;int hsh = screenh / 2;
    window.setPosition(sf::Vector2i(screenw/2-150, screenh/2-150));
    sf::Font font = sf::Font();
    font.loadFromFile("data/PixelifySans-Medium.ttf");
    bool start = false;
    int snakex = hsw - (hsw % 10);
    int snakey = hsh - (hsw % 10);
    int snakevx = 0;
    int snakevy = 0;
    int score = 0;
    std::vector<Point> apples = std::vector<Point>();
    std::vector<Point> trail = std::vector<Point>();
    // Get random seed
    FILE* rs = fopen("/dev/urandom", "rb");
    char seed = 0;
    fread(&seed, 1, 1, rs);
    fclose(rs);
    srand(seed);
    char* scored = (char*)malloc(100);
    float easingt = 0;

    for (int i = 0; i < 20; i++) {
        int lx = random()%((screenw-40)/20) * 20 + 20;
        int ly = random()%((screenh-40)/20) * 20 + 20;
        apples.push_back(Point(lx, ly));
    }

    while (window.isOpen())
    {
        easingt += 0.001;
        if (easingt >= 1) {
            easingt = 0;
        }
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        sf::Vector2i winpos = window.getPosition();

        window.clear();
        if (!start) {
            sf::Text text_start;
            text_start.setString("Press enter to start\nHint:\ndata on top of screen");
            text_start.setCharacterSize(24);
            text_start.setFont(font);
            text_start.setPosition(sf::Vector2f(10, 10));

            window.draw(text_start);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
                start = true;
            }
        }
        else {
            if (trail.size() >= score + 3) {
                trail.erase(trail.begin());
            }
            for (int i = 0; i < screenw; i += 20) {
                if (i - winpos.x < 0 || i - winpos.x > 300) {
                    continue;
                }
                sf::RectangleShape line = sf::RectangleShape();
                line.setSize(sf::Vector2f(1, 300));
                line.setFillColor(sf::Color(100, 100, 100));
                line.setPosition(sf::Vector2f(i - winpos.x, 0));
                window.draw(line);
                line.setFillColor(sf::Color(50, 50, 50));
                line.setPosition(sf::Vector2f(i - winpos.x-1, 0));
                window.draw(line);
            }
            for (int i = 0; i < screenh; i += 20) {
                if (i - winpos.y < 0 || i - winpos.y > 300) {
                    continue;
                }
                sf::RectangleShape line = sf::RectangleShape();
                line.setSize(sf::Vector2f(300, 1));
                line.setFillColor(sf::Color(100, 100, 100));
                line.setPosition(sf::Vector2f(0, i - winpos.y));
                window.draw(line);
                line.setFillColor(sf::Color(50, 50, 50));
                line.setPosition(sf::Vector2f(0, i - winpos.y+1));
                window.draw(line);
            }
            snakex -= snakex % 10;
            snakey -= snakey % 10;
            float e = easeOutExpo(easingt);
            window.setPosition(
                sf::Vector2i(
                        (int)
                        (
                            (
                                (float)snakex - 150
                            ) 
                            *
                            e
                            + 
                            (float)winpos.x 
                            * 
                            (1-e)
                        ), 
                        
                        (int)
                        (
                            (
                                (float)snakey - 150
                            ) 
                            *
                            e
                            + 
                            (float)winpos.y 
                            * 
                            (1-e)
                        )
                    )
                );
            sf::CircleShape snake = sf::CircleShape(10);
            snake.setPosition(sf::Vector2f(snakex - winpos.x-1, snakey - winpos.y+1));
            snake.setFillColor(sf::Color(100, 100, 100));
            window.draw(snake);
            snake.setPosition(sf::Vector2f(snakex - winpos.x, snakey - winpos.y));
            snake.setFillColor(sf::Color(255, 255, 255));
            window.draw(snake);
            if (glow) {
                for (int i = 0; i < 10; i++) {
                    float r = snake.getRadius()+1;
                    snake.setRadius(r);
                    snake.setPosition(sf::Vector2f(snakex - winpos.x-i, snakey - winpos.y-i));
                    snake.setFillColor(sf::Color(255, 255, 255, (100-i)/10));
                    window.draw(snake);
                }
            }
            for (int i = 0; i < trail.size(); i++) {
                sf::CircleShape snake = sf::CircleShape(10);
                snake.setPosition(sf::Vector2f(trail[i].x - winpos.x-1, trail[i].y - winpos.y+1));
                snake.setFillColor(sf::Color(100, 100, 100));
                window.draw(snake);
                snake.setPosition(sf::Vector2f(trail[i].x - winpos.x, trail[i].y - winpos.y));
                snake.setFillColor(sf::Color(255, 255, 255));
                window.draw(snake);
                if (glow) {
                    for (int j = 0; j < 10; j++) {
                        float r = snake.getRadius()+1;
                        snake.setRadius(r);
                        snake.setPosition(sf::Vector2f(trail[i].x - winpos.x-j, trail[i].y - winpos.y-j));
                        snake.setFillColor(sf::Color(255, 255, 255, (100-j)/10));
                        window.draw(snake);
                    }
                }
            }
            snakex += snakevx;
            snakey += snakevy;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
                snakevx = 0; snakevy = -20;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                snakevx = -20; snakevy = 0;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                snakevx = 0; snakevy = 20;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                snakevx = 20; snakevy = 0;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::O)) {
                FILE* save = fopen("data/save.bin", "wb");
                short* values = (short*)malloc(2 * 24); // score, snakex, snakey, seed, apples
                values[0] = score;
                values[1] = snakex;
                values[2] = snakey;
                values[3] = seed;
                for (int i = 0; i < 10; i++) {
                    values[4+i*2] = apples[i].x;
                    values[5+i*2] = apples[i].y;
                }
                fwrite(values, 2, 24, save);
                fclose(save);
                free(values);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
                FILE* save = fopen("data/save.bin", "rb");
                short* values = (short*)malloc(2 * 24); // score, snakex, snakey, seed, apples
                fread(values, 2, 24, save);fclose(save);
                snakevx = 0;
                snakevy = 0;
                score = values[0];
                snakex = values[1];
                snakey = values[2];
                seed = values[3];
                for (int i = 0; i < 10; i++) {
                    apples[i].x = values[4+i*2];
                    apples[i].y = values[5+i*2];
                }
                free(values);
            }
            for (int i = 0; i < apples.size(); i++) {
                Point apple = apples[i];
                sf::CircleShape appleo = sf::CircleShape(10);
                appleo.setPosition(sf::Vector2f(apple.x - winpos.x - 1, apple.y - winpos.y + 1));
                appleo.setFillColor(sf::Color(100, 0, 0));
                window.draw(appleo);
                appleo.setPosition(sf::Vector2f(apple.x - winpos.x, apple.y - winpos.y));
                appleo.setFillColor(sf::Color::Red);
                window.draw(appleo);
                if (glow) {
                    for (int j = 0; j < 10; j++) {
                        float r = appleo.getRadius()+1;
                        appleo.setRadius(r);
                        appleo.setPosition(sf::Vector2f(apple.x - winpos.x-j, apple.y - winpos.y-j));
                        appleo.setFillColor(sf::Color(255, 0, 0, (100-j)/10));
                        window.draw(appleo);
                    }
                }
                if (sqrtf(powf(snakex - apple.x, 2) + powf(snakey - apple.y, 2)) < 10) {
                    int lx = random()%((screenw-40)/20) * 20 + 20;
                    int ly = random()%((screenh-40)/20) * 20 + 20;
                    apples[i] = Point(lx, ly); 
                    score ++;
                }
            }
            if (winpos.y < 70) {
                sprintf(scored, "Score: %d, Window Pos: (%d, %d)", score, winpos.x, winpos.y);
                sf::Text scoredo = sf::Text(scored, font);
                scoredo.setPosition(sf::Vector2f(hsw-winpos.x, 40-winpos.y));
                window.draw(scoredo);
            }
            if (snakevx != 0 || snakevy != 0) {
                trail.push_back(Point(snakex, snakey));
            }
        }
        window.display();
        usleep(60000);
    }

    return 0;
}