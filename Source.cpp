#include <SFML/Graphics.hpp>
#include <cmath>

struct Player
{
    float x, y;
    float angle;
};

Player player = {200.0f, 200.0f, 0.0f};

const int screenWidth = 1280;
const int screenHeight = 720;
int mapSize = 1024;

const float playerSpeed = 0.01f;
const float rotationSpeed = 0.1f;
const float nearPlane = 0.1f;
const float farPlane = 1000.0f;
const float fieldOfView = 120.0f;
const float M_PI = 3.14159265358979323846f;

float fWorldX = player.x;
float fWorldY = player.y;
float fWorldA = player.angle;
float fNear = 0.005f;
float fFar = 0.05f;
float fFoVHalf = M_PI / 4.0f;

sf::VertexArray vertices(sf::Points, screenWidth *screenHeight);

sf::Image image;

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

    // Precalculate trigonometric values for the player's angle
    float cosA = std::cos(player.angle);
    float sinA = std::sin(player.angle);

    // Precalculate the difference and ratio for the frustum corner points
    float diffX1 = fFarX1 - fNearX1;
    float diffY1 = fFarY1 - fNearY1;
    float diffX2 = fFarX2 - fNearX2;
    float diffY2 = fFarY2 - fNearY2;

    int index = 0; // Index for the vertex array

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

            // Update the vertex position
            vertices[index].position = sf::Vector2f(x, y + (screenHeight / 2));
            vertices[index].texCoords =
                sf::Vector2f(fSampleX * floorTexture.getSize().x, fSampleY * floorTexture.getSize().y);
            index++;
        }
    }

    sf::RenderStates states;
    states.texture = &floorTexture;
    window.draw(vertices, states);
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "Mode 7 Implementation");
    sf::Texture floorTexture;
    floorTexture.loadFromFile("floor_texture.png");
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

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape)
                {
                    window.close();
                }
            }
        }

        float cosA = std::cos(player.angle);
        float sinA = std::sin(player.angle);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            player.x += cosA * playerSpeed;
            player.y += sinA * playerSpeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            player.x -= cosA * playerSpeed;
            player.y -= sinA * playerSpeed;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            player.angle -= rotationSpeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            player.angle += rotationSpeed;
        }

        // if up arrow is pressed
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            fFar += 0.01f;
        }
        // if down arrow is pressed
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
        {
            fFar -= 0.01f;
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
