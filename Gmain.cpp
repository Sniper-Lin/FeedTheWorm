// Copyright 2021 Lin linboy@bu.edu
//Copyright 2021 Wang hwang3@bu.edu

#include<cmath>
#include <fstream>
#include <iostream>
#include <SFML/Graphics.hpp>
#include<SFML/System.hpp>
#include <SFML/Window.hpp>
#include <stdlib.h>
#include <string>
#include <thread>
#include <vector>

using namespace std;

//Game required macro variables with constant values
#define WIDTH 1152     //Width of window
#define HEIGHT 648     //Height of window
#define PSCALE 0.2          //ratio of window width for displaying game parameter data
#define SCALE 0.2       //Aspect ratio of maps
#define WORMSIZE 10.0       //Dimensions of the worm
#define FRUITSIZE  20.0        //Dimensions of the worm food
#define LMAP (PSCALE*WIDTH)        //Location of the map's left boundary
#define RMAP (WIDTH-WORMSIZE*2)    //Location of the map's right boundary
#define TMAP 0              //Location of the map's top boundary
#define BMAP (HEIGHT-WORMSIZE*2)   //Location of the map's bottom bounndary
#define SPACE 0.1               // The spacing between the lines of game texts
#define STARTPOSITION (sf::Vector2f(WIDTH*PSCALE+25, 25))    //starting position for worm
#define TEXTSIZE 25             //text size
#define GROWTHNUM 100            //for the worm
#define STARTSPEED 0.3           //standard game speed
#define WORMCOLOR (sf::Color(163,181,123))     //color of worm

#define MCENTER (sf::Vector2f(WIDTH*(PSCALE + SCALE/2), HEIGHT*SCALE/2))    //Center of the main view
#define MVIEWSIZE (sf::Vector2f(SCALE*WIDTH, SCALE*HEIGHT)) //Scaling (zooming in) map so the main view is centered on the location of the worm
#define MVIEWPORT (sf::FloatRect(0.2, 0, 0.8f, 1.f))   //left corner position and size of view in window

#define PARAMCENTER (sf::Vector2f(WIDTH*PSCALE/2, 0.5*HEIGHT))   //Parameters for the window's left column displaying game texts 
#define PARAMVIEWSIZE (sf::Vector2f(0.2*WIDTH, HEIGHT))
#define PARAMVIEWPORT (sf::FloatRect(0, 0, 0.2f, 1.f))

#define MINCENTER (sf::Vector2f((PSCALE+(1-PSCALE)/2)*WIDTH, 0.5*HEIGHT)) //Center of the small map
#define MINVIEWSIZE (sf::Vector2f((1-PSCALE)*WIDTH, HEIGHT))
#define MINVIEWPORT (sf::FloatRect(1-SCALE, 0, SCALE, SCALE))

//Declaration of the classes
class Worm;
class GameParameter;
class Fruit;

//Declaration  of functions
int game_start(); //initialize game
bool checkCrash(sf::Vector2f headport); //check if the worm head crashed against map border
void draw(bool act); //Drawing the game texts, backgrounds, and objects
void setLevel(int l); //change between different game speeds
float round_float(float x, float minimum, float maximum); // make sure the main scaled view moves at the right pace with the worm


//Declaration of elements in the gaming window
sf::RenderWindow window;
sf::Font font;
sf::Clock gameclock;  //for displaying the total run time of the game
sf::View FocusedView;
sf::View Parameter;
sf::View miniView;
sf::Texture texture;
sf::Texture texture1;
sf::Sprite sprite;
sf::Sprite sprite1;
sf::Text Message;
sf::RectangleShape box;
int Level = 3;
int BestScore = 0;
bool pause = false;
extern Worm worm;
const double PI = 3.14159;

class Worm {
 public:
  int length;
  float speed;
  bool dead;
  sf::Vector2f direction;  //direction of travel
  sf::Vector2f head;    //position of worm head
  std::vector<sf::Vertex> vertices_worm; //vector of vertices where worm bodies are connected
  static float responsiveness; //of worm turning directions
  static float displacement; //distance covered by worm
  static int growthNum;    //constant growth for worm

  Worm() {
    this->respawn();
  }

  void respawn() {
    // for restarting the game
    this->length = 0;
    this->dead = false;
    this->speed = STARTSPEED * Level;
    this->direction = sf::Vector2f(1, 0);
    this->head = STARTPOSITION;
    std::vector<sf::Vertex> v;  //creating new worm
    this->vertices_worm = v;

    Message.setString("Now  Playing");
    cout << "Restart" << endl;
  }

  sf::Vector2f getWormHeadPosition() {
    return this->head;
  }

  void updateVertex(sf::Time T) {
    //Make sure the positions of the worm bodies get updated and moves together 
    if (worm.dead || pause)
      this->speed = 0;
    else
      this->speed = STARTSPEED * Level;
    sf::Vector2f headpos;
    sf::Vertex v_temp;
    v_temp.color = sf::Color::Black;
    std::vector<sf::Vertex>::iterator it;

    if (this->vertices_worm.size() > 0) {    // After the fruit eaten is greater than zero
      for (int i = 0; i < (int)(this->speed * T.asMilliseconds()); i++) {
        headpos = this->vertices_worm.back().position;
        headpos = headpos + this->direction * this->displacement; // The new position of the worm head after each movement
        v_temp.position = headpos;
        this->vertices_worm.push_back(v_temp);  //place new vertex at the back
        it = this->vertices_worm.begin();  //Change the iterator to the beginning of worm vertex vector
        this->vertices_worm.erase(it++); //erase the last position of the back of worm

        this->head = this->head + this->direction * this->displacement;
      }
    } else {
      for (int i = 0; i < (int)(this->speed * T.asMilliseconds()); i++) {
        headpos = this->head;
        headpos = headpos + this->direction * this->displacement;
        this->head = headpos;
      }
    }
  }

  void draw() {
    //for drawing the worm
    sf::CircleShape Ctemp(WORMSIZE);
    Ctemp.setPosition(this->head);
    Ctemp.setFillColor(WORMCOLOR);
    Ctemp.setScale(sf::Vector2f(1.f, 0.8f));
    window.draw(Ctemp);
    for (int i = 0; i < this->vertices_worm.size(); i++) {
      if (i!=0 && i % GROWTHNUM == 0) {
        Ctemp.setPosition(this->vertices_worm[i].position);
        window.draw(Ctemp);
      }
    }
  }

  void grow() {         //grow worm with new vertices that specficy where new bodies are joined
    float x, y;
    sf::Vector2f v_temp1;
    sf::Vector2f v_temp2;
    sf::Vertex v_temp;
    v_temp.color = sf::Color::Black;
    for (int i = 0; i < this->growthNum; i++) {
      v_temp1 = this->head;
      v_temp2 = v_temp1 + this->direction * this->displacement; //postion of the head of the worm after each movement
      this->head = v_temp2;
      v_temp.position = v_temp2;
      this->vertices_worm.push_back(v_temp);  //place at back of the worm vertices vector
    }
    this->length++;
    cout << "Worm growing..." << endl;
  }

  bool isEaten(sf::RectangleShape rect) {
    sf::CircleShape Ctemp(WORMSIZE);
    Ctemp.setPosition(this->head);
    sf::FloatRect box1 = rect.getGlobalBounds();
    sf::FloatRect box2 = Ctemp.getGlobalBounds();
    return box1.intersects(box2);
  }

  bool isCrash() { //Check if worm crashed against the map border
    sf::Vector2f headport;
    headport = this->head;
    return checkCrash(headport);
  }
};

class GameParameter {
 public:
  
  sf::Text gameName;
  sf::Text up_time;
  sf::Text levelTxt;
  sf::Text best_score;
  sf::Text best_score_Txt;
  sf::Text length;
  sf::Text speedTxt;
  sf::Text level;    //Levels one to nine associated with different speeds
  sf::Text up_time_Txt;
  sf::Text lengthTxt;
  sf::Text speed_Txt;

  GameParameter() {

    this->gameName = sf::Text("Feed the Worm", font, 45);
    this->gameName.setPosition(sf::Vector2f(0.02 * WIDTH, 0.03 * HEIGHT));
    this->gameName.setFillColor(sf::Color::Black);

    this->up_time_Txt = sf::Text("Elapsed  Time:", font, TEXTSIZE);
    this->up_time_Txt.setPosition(sf::Vector2f(0.02 * WIDTH, 0.08 * HEIGHT + 1 * HEIGHT * SPACE));
    this->up_time = sf::Text("0.00", font, TEXTSIZE);
    this->up_time.setPosition(sf::Vector2f(0.125 * WIDTH, 0.08 * HEIGHT + 1 * HEIGHT * SPACE));
    this->up_time_Txt.setFillColor(sf::Color::Black);
    this->up_time.setFillColor(sf::Color::Black);

    this->levelTxt = sf::Text("Difficulty  level: ", font, TEXTSIZE);
    this->levelTxt.setPosition(sf::Vector2f(0.02 * WIDTH, 0.08 * HEIGHT + 2 * HEIGHT * SPACE));
    this->level = sf::Text("1", font, TEXTSIZE);
    this->level.setPosition(sf::Vector2f(0.12 * WIDTH, 0.08 * HEIGHT + 2 * HEIGHT * SPACE));
    this->levelTxt.setFillColor(sf::Color::Black);
    this->level.setFillColor(sf::Color::Black);

    this->speed_Txt = sf::Text("Game  Speed: ", font, TEXTSIZE);
    this->speed_Txt.setPosition(sf::Vector2f(0.02 * WIDTH, 0.08 * HEIGHT + 3 * HEIGHT * SPACE));
    this->speedTxt = sf::Text("0.00", font, TEXTSIZE);
    this->speedTxt.setPosition(sf::Vector2f(0.09 * WIDTH, 0.08 * HEIGHT + 3 * HEIGHT * SPACE));
    this->speed_Txt.setFillColor(sf::Color::Black);
    this->speedTxt.setFillColor(sf::Color::Black);

    this->lengthTxt = sf::Text("Eating  Score: ", font, TEXTSIZE);
    this->lengthTxt.setPosition(sf::Vector2f(0.02 * WIDTH, 0.08 * HEIGHT + 4 * HEIGHT * SPACE));
    this->length = sf::Text("1", font, TEXTSIZE);
    this->length.setPosition(sf::Vector2f(0.13 * WIDTH, 0.08 * HEIGHT + 4 * HEIGHT * SPACE));
    this->lengthTxt.setFillColor(sf::Color::Black);
    this->length.setFillColor(sf::Color::Black);

    this->best_score_Txt = sf::Text("Best  Score: ", font, TEXTSIZE);
    this->best_score_Txt.setPosition(sf::Vector2f(0.02 * WIDTH, 0.08 * HEIGHT + 5 * HEIGHT * SPACE));
    this->best_score = sf::Text("0", font, TEXTSIZE);
    this->best_score.setPosition(sf::Vector2f(0.14 * WIDTH, 0.08 * HEIGHT + 5 * HEIGHT * SPACE));
    this->best_score_Txt.setFillColor(sf::Color::Black);
    this->best_score.setFillColor(sf::Color::Black);

    cout << "Establishing game data" << endl;
  }

  void inData() { //Game data updating

    char buffer_1[15] = { 0 };
    sprintf(buffer_1, "%.3fs", gameclock.getElapsedTime().asSeconds());
    this->up_time.setString(buffer_1);

    char buffer_2[10] = { 0 };
    sprintf(buffer_2, "%d", worm.length);
    this->length.setString(buffer_2);

    char buffer_3[10] = { 0 };
    sprintf(buffer_3, "%.2f", worm.speed);
    this->speedTxt.setString(buffer_3);

    char buffer_4[10] = { 0 };
    sprintf(buffer_4, "%d", Level);
    this->level.setString(buffer_4);

    if (BestScore < worm.length) {
      BestScore = worm.length;
      char buffer_5[10] = { 0 };
      sprintf(buffer_5, "%d", BestScore);
      this->best_score.setString(buffer_5);
    }
  }

  void draw() {

    window.draw(this->gameName);
    window.draw(this->up_time_Txt);
    window.draw(this->up_time);
    window.draw(this->levelTxt);
    window.draw(this->level);
    window.draw(this->speed_Txt);
    window.draw(this->speedTxt);
    window.draw(this->lengthTxt);
    window.draw(this->length);
    window.draw(this->best_score_Txt);
    window.draw(this->best_score);
  }
};

class Fruit {
 public:

  sf::RectangleShape rect;

  Fruit() {

  }

  void start() {
    this->createFruit();
  }

  void createFruit() {
    sf::Vector2f V_temp(0, 0);
    srand((unsigned int)time(0)); //randomize the placement of the food
    this->rect.setSize(sf::Vector2f(FRUITSIZE, FRUITSIZE));
    this->rect.setFillColor(sf::Color::Magenta);
    this->rect.setScale(sf::Vector2f(1.f, 0.8f));
    V_temp = sf::Vector2f(rand() % (int)((1 - PSCALE) * WIDTH) + PSCALE * WIDTH, rand() % (int)HEIGHT);
    V_temp.x = round_float(V_temp.x, WIDTH * PSCALE + 30, WIDTH - 30);
    V_temp.y = round_float(V_temp.y, 30, HEIGHT - 30);
    this->rect.setPosition(V_temp);
    cout << "Create worm food" << endl;
  }

  void draw() {
    window.draw(this->rect);
  }

};

GameParameter gameparameter;
Worm worm;
Fruit fruit;
int Worm::growthNum = GROWTHNUM;
float Worm::responsiveness = 1.5;
float Worm::displacement = 1.5 * WORMSIZE / GROWTHNUM;

void setView() { 
  //Initializing the main map, mini map, and parameter view
  FocusedView.setCenter(MCENTER);
  FocusedView.setSize(MVIEWSIZE);
  FocusedView.setViewport(MVIEWPORT);

  Parameter.setCenter(PARAMCENTER);
  Parameter.setSize(PARAMVIEWSIZE);
  Parameter.setViewport(PARAMVIEWPORT);

  miniView.setCenter(MINCENTER);
  miniView.setSize(MINVIEWSIZE);
  miniView.setViewport(MINVIEWPORT);
}

float round_float(float x, float minimum, float maximum) {
  if (x < minimum)
    return minimum;
  else if (x > maximum)
    return maximum;
  else
    return x;
}

int game_start() {
  //Create window and food
  window.create(sf::VideoMode(WIDTH, HEIGHT), "EC327_Project Feed the Worm Game");
  fruit.start();

  setView(); 
  
  if (!font.loadFromFile("./font/Amatic-Bold.ttf")) {
    cout << "Failed to load text font" << endl;
    return -1;
  }

  //load in game backgrounds
  texture.loadFromFile("./background.jpg");
  sprite.setPosition(sf::Vector2f(LMAP, TMAP));
  sprite.setScale(sf::Vector2f(.8, 1.5));
  sprite.setTexture(texture);
  texture.setSmooth(true);

  texture1.loadFromFile("./backgrounds/sky.jpg");
  sprite1.setPosition(sf::Vector2f(0, 0));
  sprite1.setScale(sf::Vector2f(.366, 2));
  sprite1.setTexture(texture1);
  texture1.setSmooth(true);


  //initializing message
  Message.setString("Initializing...");
  Message.setFont(font);
  Message.setCharacterSize(30);
  Message.setPosition(sf::Vector2f(0.02 * WIDTH, 0.7 * HEIGHT));
  Message.setFillColor(sf::Color::Black);

  //moving box centered on the worm
  box.setFillColor(sf::Color(0, 0, 0, 0));
  box.setOutlineThickness(4);
  box.setOutlineColor(sf::Color::Black);
  box.setSize(sf::Vector2f(WIDTH * SCALE + 20, HEIGHT * SCALE + 20));
  box.setPosition(sf::Vector2f(WIDTH * PSCALE + 10, +10));
  return 0;
}

void draw(bool act) {
  window.draw(sprite);
  window.draw(sprite1);
  gameparameter.draw();
  worm.draw();
  fruit.draw();
  if (worm.dead) {
    Message.setString("Worm  died\nPress  Enter  to  respawn"); //Game messages printed when worm dies
  }
  window.draw(Message);
  if(act)
    window.draw(box);
}

bool checkCrash(sf::Vector2f headport) {
  if (headport.x < LMAP || headport.x > RMAP || headport.y < TMAP || headport.y > BMAP)
    return 1;
  else
    return 0;
}

void setLevel(int l) {
  char buffer[10] = { 0 };
  Level = l;
  sprintf(buffer, "%d", l);
  gameparameter.level.setString(buffer);
  if (worm.dead || pause)
    worm.speed = 0;
  else
    worm.speed = STARTSPEED * Level;
}



int main() {
  game_start();
  sf::Clock clock_game;
  Message.setString("Now  Playing");


  while (window.isOpen()) {
    sf::Vector2f centertemp(0, 0);

    //update data
    worm.updateVertex(clock_game.restart());
    gameparameter.inData();
    gameparameter.draw();

    //update the focused view's location on map
    centertemp = worm.getWormHeadPosition();
    centertemp.x = round_float(centertemp.x, WIDTH * (PSCALE + SCALE / 2), WIDTH * (1 - SCALE / 2));
    centertemp.y = round_float(centertemp.y, HEIGHT * SCALE / 2, HEIGHT * (1 - SCALE / 2));
    FocusedView.setCenter(centertemp);
    box.setPosition(centertemp - sf::Vector2f(WIDTH * SCALE / 2 + 10, HEIGHT * SCALE / 2 + 10));

    if (worm.isEaten(fruit.rect)) {    //if worm catches fruit
      cout << "Successfully eaten and digested" << endl;
      worm.grow();
      fruit.createFruit();
    }
    if (worm.isCrash()) {        //game ends if worm crashes map border
      worm.dead = true;
      Message.setString("Worm died\nPress Enter to respawn\n");
      cout << "Crashed against border" << endl;
    }

    
    sf::Event event;
    //using event to assign game controls

    while (window.pollEvent(event)) {
      sf::Uint64 input =0;
      input=event.text.unicode;

      switch (event.type) {
        case sf::Event::Closed:
          window.close();

        case sf::Event::TextEntered:
          if (input < 130)
            cout << (char)input << endl;

        switch (input) {
        case 'Q':
        case 'q':
          //print center positions of the focused view
          cout << "x coordinate of center: " << FocusedView.getCenter().x << endl;
          cout << "y coordinate of center: " << FocusedView.getCenter().y << endl;
          break;

        case 'A':
        case 'a':
          //Move worm to the left
          worm.direction = worm.direction + worm.responsiveness * sf::Vector2f(-1, 0);
          worm.direction = worm.direction / (sqrt(worm.direction.x * worm.direction.x + worm.direction.y * worm.direction.y));
          break;

        case 'D':
        case 'd':
          //Move worm to the right
          worm.direction = worm.direction + worm.responsiveness * sf::Vector2f(1, 0);
          worm.direction = worm.direction / (sqrt(worm.direction.x * worm.direction.x + worm.direction.y * worm.direction.y));
          break;

        case 'W':
        case 'w':
          //Move worm up
          worm.direction = worm.direction + worm.responsiveness * sf::Vector2f(0, -1);
          worm.direction = worm.direction / (sqrt(worm.direction.x * worm.direction.x + worm.direction.y * worm.direction.y));
          break;

        case 'S':
        case 's':
          //Move worm down
          worm.direction = worm.direction + worm.responsiveness * sf::Vector2f(0, 1);
          worm.direction = worm.direction / (sqrt(worm.direction.x * worm.direction.x + worm.direction.y * worm.direction.y));
          break;

        case 0x0D:
          //Press enter for restarting the game
          if(worm.dead)
            worm.respawn();
          break;

        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
          //setting game difficulty
          setLevel(input - '0');
          break;

        case ' ':
          //pause the game with the space button
          pause = !pause;
          break;

        default:
          break;
        }
        break;
      default:
        break;
      }
    }

    //draw
    window.clear();
    window.setView(FocusedView);
    draw(false);
    window.setView(Parameter);
    draw(false);
    window.setView(miniView);
    draw(true);
    window.display();
  }
  return 0;
}
