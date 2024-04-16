#include <SFML/Graphics.hpp>
#include <cmath>

const float M_PI = 3.14159265358979323846f;

const int screenWidth = 400;
const int screenHeight = 400;
const float playerSpeed = 0.01f;
const float rotationSpeed = 0.1f;
const float nearPlane = 0.1f;
const float farPlane = 1000.0f;
const float fieldOfView = 90.0f;

struct Player
{
    float x, y;
    float angle;
};

Player player = {200.0f, 200.0f, 0.0f};

float fWorldX = player.x;
float fWorldY = player.y;
float fWorldA = player.angle;
float fNear = 0.005f;
float fFar = 0.03f;
float fFoVHalf = 3.14159f / 4.0f;
int mapSize = 1024;
sf::Image image;
sf::RectangleShape pixel(sf::Vector2f(1, 1));

void Update(sf::Texture &floorTexture, sf::RenderWindow &window, sf::Image &image)
{

    // Create Frustum corner points
    float fFarX1 = fWorldX + cosf(fWorldA - fFoVHalf) * fFar;
    float fFarY1 = fWorldY + sinf(fWorldA - fFoVHalf) * fFar;

    float fNearX1 = fWorldX + cosf(fWorldA - fFoVHalf) * fNear;
    float fNearY1 = fWorldY + sinf(fWorldA - fFoVHalf) * fNear;

    float fFarX2 = fWorldX + cosf(fWorldA + fFoVHalf) * fFar;
    float fFarY2 = fWorldY + sinf(fWorldA + fFoVHalf) * fFar;

    float fNearX2 = fWorldX + cosf(fWorldA + fFoVHalf) * fNear;
    float fNearY2 = fWorldY + sinf(fWorldA + fFoVHalf) * fNear;

    sf::Color col; // Reusable sf::Color object

    // Precalculate trigonometric values for the player's angle
    float cosA = std::cos(player.angle);
    float sinA = std::sin(player.angle);

    // Precalculate the difference and ratio for the frustum corner points
    float diffX1 = fFarX1 - fNearX1;
    float diffY1 = fFarY1 - fNearY1;
    float diffX2 = fFarX2 - fNearX2;
    float diffY2 = fFarY2 - fNearY2;

    // Starting with furthest away line and work towards the camera point
    for (int y = 0; y < screenHeight / 2; y++)
    {
        // Precalculate the sample depth ratio
        float fSampleDepth = (float)y / (float)(screenHeight / 2);

        // Precalculate start and end points for lines across the screen
        float fStartX = diffX1 / fSampleDepth + fNearX1;
        float fStartY = diffY1 / fSampleDepth + fNearY1;
        float fEndX = diffX2 / fSampleDepth + fNearX2;
        float fEndY = diffY2 / fSampleDepth + fNearY2;

        // Linearly interpolate lines across the screen
        for (int x = 0; x < screenWidth; x++)
        {
            float fSampleWidth = (float)x / (float)screenWidth;
            float fSampleX = (fEndX - fStartX) * fSampleWidth + fStartX;
            float fSampleY = (fEndY - fStartY) * fSampleWidth + fStartY;

            // Wrap sample coordinates to give "infinite" periodicity on maps
            fSampleX = fmod(fSampleX, 1.0f);
            fSampleY = fmod(fSampleY, 1.0f);

            // Update the existing color object
            col = image.getPixel(fSampleX * floorTexture.getSize().x, fSampleY * floorTexture.getSize().y);

            pixel.setPosition(x, y + (screenHeight / 2));
            pixel.setFillColor(col);
            window.draw(pixel);
        }
    }
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "Mode 7 Implementation");
    sf::Texture floorTexture;
    sf::Sprite floorSprite;
    floorTexture.loadFromFile("floor_texture.png");
    floorSprite.setTexture(floorTexture);
    image = floorTexture.copyToImage();

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        // Update player position based on angle and speed
        float cosA = std::cos(player.angle * M_PI / 180.0f);
        float sinA = std::sin(player.angle * M_PI / 180.0f);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            player.x += cosA * playerSpeed;
            player.y += sinA * playerSpeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            player.x -= std::cos(player.angle * M_PI / 180.0f) * playerSpeed;
            player.y -= std::sin(player.angle * M_PI / 180.0f) * playerSpeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            player.angle -= rotationSpeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            player.angle += rotationSpeed;
        }

        fWorldX = player.x;
        fWorldY = player.y;
        fWorldA = player.angle;

        window.clear(sf::Color::Black);

        Update(floorTexture, window, image);
        window.display();
    }

    return 0;
}
