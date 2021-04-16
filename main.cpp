#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <SFML/Audio.hpp>
using namespace std;

const int c1=15, c2=30, c3=35, c4=100; /// costs of towers
const int easy=70, hard=66;

string to_str(int val)
{
    ostringstream x;
    x << val;
    return x.str();
}

class Guy
{
public:
    int x, y;
    int orgT;

    sf::Sprite sprite;

    Guy(int OT, sf::Texture &tex) : orgT(OT)
    {
        sprite.setTexture(tex);
        sprite.setOrigin(tex.getSize().x/2, tex.getSize().y/2);
    }

    double X(double t)
    {
        return (t-orgT)*0.5;//+sin((t-orgT)/180*M_PI)*64;
    }

    double Y(double t)
    {
        return (sin((t-orgT)/180*M_PI*1)*128)*0.5+320;
    }

    void step(double t)
    {
        x=X(t);
        y=Y(t);

        sprite.setPosition(x,y);
    }

    void draw(sf::RenderWindow &app)
    {
        app.draw(sprite);
    }
};

class You
{
public:
    int x, y;
    int cooldown;
    bool side;

    sf::Sprite sprite;

    You() {}
    You(sf::Texture &tex, int X, int Y) : x(X), y(Y), cooldown(0), side(1)
    {
        sprite.setTexture(tex);
        sprite.setOrigin(tex.getSize().x/2, tex.getSize().y/2);
        sprite.setPosition(x,y);
    }

    bool step()
    {
        if (cooldown>0)
            cooldown--;
        sprite.setPosition(x,y);
        return cooldown==0;
    }

    void draw(sf::RenderWindow &app)
    {
        app.draw(sprite);
    }
};

class Enemy
{
public:
    double x, y;
    int t, tmax;
    bool immune;

    sf::Sprite sprite;

    Enemy(double X, double Y, bool IMM, sf::Texture &tex) : x(X), y(Y), immune(IMM), tmax(180)
    {
        t=tmax;
        sprite.setTexture(tex);
        sprite.setOrigin(tex.getSize().x/2, tex.getSize().y/2);
        sprite.setPosition(x,y);
    }

    bool step()
    {
        if (t>0)
            t--;
        sprite.setPosition(x,y);
        return t==0;
    }

    void draw(sf::RenderWindow &app)
    {
        app.draw(sprite);
    }
};

class Tower
{
public:
    double x, y;
    int t, maxt;
    int range;

    sf::Sprite sprite;

    Tower(double X, double Y, int MAXT, int RANGE, sf::Texture &tex) : x(X), y(Y), maxt(MAXT), range(RANGE)
    {
        t=maxt;
        sprite.setTexture(tex);
        sprite.setOrigin(tex.getSize().x/2, tex.getSize().y/2);
        sprite.setPosition(x,y);
    }

    void step()
    {
        if (t>0)
            t--;
    }

    void draw(sf::RenderWindow &app)
    {
        app.draw(sprite);
    }
};

class Line
{
public:
    sf::Vertex line[2];
    int timer;

    Line(double x1, double y1, double x2, double y2, int TIMER) : timer(TIMER)
    {
        line[0] = sf::Vertex(sf::Vector2f(x1, y1));
        line[0].color = sf::Color::Yellow;
        line[1] = sf::Vertex(sf::Vector2f(x2, y2));
        line[1].color = sf::Color::Magenta;
    };

    bool step()
    {
        timer--;
        return timer==0;
    }

    void draw(sf::RenderWindow &app)
    {
        app.draw(line, 2, sf::Lines);
    }
};

class Anim
{
public:
    double x, y;
    int t;
    vector<sf::Texture*> anims;
    int kt;

    sf::Sprite sprite;

    Anim(double X, double Y, vector<sf::Texture*> &tex) : x(X), y(Y), t(12), kt(0)
    {
        anims = tex;
        sprite.setTexture(*anims[kt]);
        sprite.setOrigin(sprite.getGlobalBounds().width/2, sprite.getGlobalBounds().height/2);
        sprite.setPosition(x,y);
    }

    bool step()
    {
        if (t>0)
            t--;
        if (t%3==0)
        {
            kt++;
            sprite.setTexture(*anims[kt]);
        }
        return t<=0;
    }

    void draw(sf::RenderWindow &app)
    {
        app.draw(sprite);
    }
};

class Bullet
{
public:
    double x, y;
    double vx, vy;

    sf::Sprite sprite;

    Bullet(double X, double Y, double VX, double VY, sf::Texture &tex) : x(X), y(Y), vx(VX), vy(VY)
    {
        sprite.setTexture(tex);
        sprite.setOrigin(tex.getSize().x/2, tex.getSize().y/2);
        sprite.setPosition(x,y);
    }

    void step()
    {
        x+=vx;
        y+=vy;
        sprite.setPosition(x,y);
    }

    void draw(sf::RenderWindow &app)
    {
        app.draw(sprite);
    }
};

class Game
{
public:
    vector<Guy> guys;
    vector<Enemy> enemies;
    vector<Tower> towers;
    vector<Line> lines;
    vector<Anim> anims;
    vector<Tower> towersM;
    vector<Enemy> enemies2;
    vector<Bullet> bullets;
    vector<Anim> anims2;
    You you;
    sf::Sprite bcg;
    sf::Sprite fire;
    sf::Sprite gameover;
    sf::Sprite pointer;
    sf::Sprite buy1, buy2, buy3, buy4;
    sf::Sprite locked1, locked2, locked3, locked4;
    vector<sf::Texture> hub;
    vector<sf::Texture*> blod, magic, glow;
    sf::Font font;
    sf::Text sScore, sc1, sc2, sc3, sc4;
    sf::Sprite menu;
    sf::Sprite youwon;
    sf::Sprite tut;
    sf::Sprite statue;

    sf::SoundBuffer buffer, buffer2, buffer3;
    sf::Sound zzt, szt, elo;

    long long int timer;
    int score;
    int tryb; /// 0-game 1-menu 2-gameover 3-youwon
    int spawn;
    int buying;
    int SPAWN;
    bool unmuted;

    void restart()
    {
        guys.clear();
        enemies.clear();
        towers.clear();
        lines.clear();
        anims.clear();
        towersM.clear();
        enemies2.clear();
        bullets.clear();
        anims2.clear();
        guys.push_back(Guy(-64, hub[1]));
        guys.push_back(Guy(0, hub[1]));
        guys.push_back(Guy(64, hub[1]));
        //enemies.push_back(Enemy(140, 10, 0, hub[3]));
        //enemies.push_back(Enemy(140, 630, 0, hub[3]));
        //enemies.push_back(Enemy(580, 630, 0, hub[3]));
        //enemies.push_back(Enemy(580, 10, 0, hub[3]));
        //towers.push_back(Tower(400, 200, 120, hub[5]));
        you = You(hub[2], 320,320);
        sScore.setString("fresh sacrifices: 0");
        timer=0;
        score=0;
        tryb=0;
        spawn=SPAWN;
        buying=-1;
    }

    Game(vector<sf::Texture> &HUB) : timer(0), score(0), tryb(1), spawn(60), SPAWN(hard), unmuted(true)
    {
        hub=HUB;
        blod.push_back(&hub[7]);
        blod.push_back(&hub[8]);
        blod.push_back(&hub[9]);
        blod.push_back(&hub[10]);
        magic.push_back(&hub[21]);
        magic.push_back(&hub[22]);
        magic.push_back(&hub[23]);
        magic.push_back(&hub[24]);
        glow.push_back(&hub[28]);
        glow.push_back(&hub[29]);
        glow.push_back(&hub[30]);
        glow.push_back(&hub[31]);
        bcg.setTexture(hub[0]);
        fire.setTexture(hub[11]);
        gameover.setTexture(hub[12]);
        buy1.setTexture(hub[15]);
        buy2.setTexture(hub[16]);
        buy3.setTexture(hub[17]);
        buy4.setTexture(hub[26]);
        locked1.setTexture(hub[36]);
        locked2.setTexture(hub[36]);
        locked3.setTexture(hub[36]);
        locked4.setTexture(hub[36]);
        font.loadFromFile("font/Calibri.ttf");
        sScore.setFont(font);
        sScore.setColor(sf::Color::White);
        sScore.setCharacterSize(30);
        sScore.setPosition(8,0);
        fire.setPosition(615, 272);
        gameover.setPosition(0, 320);
        buy1.setPosition(330,4);
        buy2.setPosition(410,4);
        buy3.setPosition(490,4);
        buy4.setPosition(570,4);
        locked1.setPosition(buy1.getPosition());
        locked2.setPosition(buy2.getPosition());
        locked3.setPosition(buy3.getPosition());
        locked4.setPosition(buy4.getPosition());
        statue.setTexture(hub[25]);
        statue.setOrigin(statue.getGlobalBounds().width/2, statue.getGlobalBounds().height/2);
        pointer.setTexture(hub[37]);
        pointer.setOrigin(pointer.getGlobalBounds().width/2, pointer.getGlobalBounds().height/2);
        sc1 = sScore;
        sc1.setCharacterSize(12);
        sc1.setPosition(330+13, 36);
        sc1.setString("15 lives");
        sc2 = sc3 = sc4 = sc1;
        sc2.setPosition(410+13, 36);
        sc2.setString("30 lives");
        sc3.setPosition(490+13, 36);
        sc3.setString("35 lives");
        sc4.setPosition(570+12-3, 36);
        sc4.setString("100 lives");
        buffer.loadFromFile("sounds/aup.wav");
        buffer2.loadFromFile("sounds/szt.wav");
        buffer3.loadFromFile("sounds/elo.wav");
        zzt.setBuffer(buffer);
        szt.setBuffer(buffer2);
        elo.setBuffer(buffer3);
        menu.setTexture(hub[38]);
        youwon.setTexture(hub[39]);
        youwon.setPosition(0,320);
        tut.setTexture(hub[40]);

        restart();
        tryb=1;
    }

    void step()
    {
        if(tryb==0)
        {
            timer++;
            for (int i=0; i<guys.size(); i++)
            {
                guys[i].step(timer);
                if (guys[i].x>640)
                {
                    score++;
                    sScore.setString("fresh sacrifices: " + to_str(score));
                    guys.erase(guys.begin()+i);
                    i--;
                }
            }

            for (int i=0; i<enemies.size(); i++)
            {
                if (guys.size()>0 && guys[0].x>0)
                {
                    double ax = guys[0].x-enemies[i].x;
                    double ay = guys[0].y-enemies[i].y;
                    double an = sqrt(ax*ax+ay*ay);
                    ax/=an;
                    ay/=an;
                    enemies[i].x+=ax;
                    enemies[i].y+=ay;
                    if (ax>=0)
                        enemies[i].immune ? enemies[i].sprite.setTexture(hub[34]) : enemies[i].sprite.setTexture(hub[3]);
                    else
                        enemies[i].immune ? enemies[i].sprite.setTexture(hub[35]) : enemies[i].sprite.setTexture(hub[6]);
                    enemies[i].step();
                    for (int j=0; j<guys.size(); j++)
                        if(abs(enemies[i].x-guys[j].x)+abs(enemies[i].y-guys[j].y)<16)
                        {
                            if (unmuted)
                                szt.play();
                            anims.push_back(Anim(guys[j].x, guys[j].y, blod));
                            guys.erase(guys.begin()+j);
                            j--;
                        }

                }
                if(abs(enemies[i].x-you.x)+abs(enemies[i].y-you.y)<=16)
                {
                    if (unmuted)
                        szt.play();
                    anims.push_back(Anim(enemies[i].x, enemies[i].y, blod));
                    enemies.erase(enemies.begin()+i);
                    i--;
                }
            }
            for (int i=0; i<enemies2.size(); i++)
            {
                double ax = you.x-enemies2[i].x;
                double ay = you.y-enemies2[i].y;
                double an = sqrt(ax*ax+ay*ay);
                if(an>=128)
                {
                    ax/=an;
                    ay/=an;
                    enemies2[i].x+=ax;
                    enemies2[i].y+=ay;
                    if (ax>=0)
                        enemies2[i].sprite.setTexture(hub[13]);
                    else
                        enemies2[i].sprite.setTexture(hub[14]);
                    enemies2[i].step();
                }
                for (int j=0; j<guys.size(); j++)
                    if(abs(enemies[i].x-guys[j].x)+abs(enemies[i].y-guys[j].y)<=16)
                    {
                        if (unmuted)
                            szt.play();
                        anims.push_back(Anim(guys[j].x, guys[j].y, blod));
                        guys.erase(guys.begin()+j);
                        j--;
                    }

                if (enemies2[i].step())
                {
                    double ax = you.x-enemies2[i].x;
                    double ay = you.y-enemies2[i].y;
                    double an = sqrt(ax*ax+ay*ay);
                    enemies2[i].t=enemies2[i].tmax;
                    ax/=an;
                    ay/=an;
                    bullets.push_back(Bullet(enemies2[i].x, enemies2[i].y, ax*5, ay*5, hub[20]));
                }

                if(abs(enemies2[i].x-you.x)+abs(enemies2[i].y-you.y)<16)
                {
                    if (unmuted)
                        szt.play();
                    anims.push_back(Anim(enemies2[i].x, enemies2[i].y, blod));
                    enemies2.erase(enemies2.begin()+i);
                    i--;
                }
            }
            for (int i=0; i<towers.size(); i++)
            {
                towers[i].step();
                if (towers[i].t==0)
                {
                    for (int j=0; j<enemies.size(); j++)
                    {
                        if (!enemies[j].immune)
                        {
                            if ((enemies[j].x-towers[i].x)*(enemies[j].x-towers[i].x)+(enemies[j].y-towers[i].y)*(enemies[j].y-towers[i].y)*2<=towers[i].range*towers[i].range)
                            {
                                if (unmuted)
                                    elo.play();
                                lines.push_back(Line(towers[i].x, towers[i].y, enemies[j].x, enemies[j].y, 8));
                                anims.push_back(Anim(enemies[j].x, enemies[j].y, blod));
                                enemies.erase(enemies.begin()+j);
                                towers[i].t=towers[i].maxt;
                                break;
                            }
                        }
                    }
                }
            }
            for (int i=0; i<towersM.size(); i++)
            {
                towersM[i].step();
                if (towersM[i].t==0)
                {
                    for (int j=0; j<enemies2.size(); j++)
                    {
                        if ((enemies2[j].x-towersM[i].x)*(enemies2[j].x-towersM[i].x)+(enemies2[j].y-towersM[i].y)*(enemies2[j].y-towersM[i].y)*2<=towersM[i].range*towersM[i].range)
                        {
                            if (unmuted)
                                elo.play();
                            lines.push_back(Line(towersM[i].x, towersM[i].y, enemies2[j].x, enemies2[j].y, 8));
                            anims.push_back(Anim(enemies2[j].x, enemies2[j].y, blod));
                            enemies2.erase(enemies2.begin()+j);
                            towersM[i].t=towersM[i].maxt;
                            break;
                        }
                    }
                }
            }
            for (int i=0; i<bullets.size(); i++)
            {
                bullets[i].step();
                if (abs(bullets[i].x-you.x)+abs(bullets[i].y-you.y)<16)
                {
                    you.cooldown=60;
                    if (SPAWN==easy)
                        you.cooldown-=20;
                    if (you.side)
                        you.sprite.setTexture(hub[32]);
                    else
                        you.sprite.setTexture(hub[33]);
                    anims2.push_back(Anim(you.x, you.y, magic));
                    bullets.erase(bullets.begin()+i);
                    if (unmuted)
                        zzt.play();
                    i--;
                }
                else if (bullets[i].x<0 || bullets[i].y<0 || bullets[i].x>640 || bullets[i].y>640)
                {
                    bullets.erase(bullets.begin()+i);
                    i--;
                }
                else
                {
                    anims.push_back(Anim(bullets[i].x, bullets[i].y, glow));
                }
            }
            for (int i=0; i<lines.size(); i++)
            {
                if (lines[i].step())
                {
                    lines.erase(lines.begin()+i);
                    i--;
                }
            }
            for (int i=0; i<anims.size(); i++)
            {
                if (anims[i].step())
                {
                    anims.erase(anims.begin()+i);
                    i--;
                }
            }
            for (int i=0; i<anims2.size(); i++)
            {
                if (anims2[i].step())
                {
                    anims2.erase(anims2.begin()+i);
                    i--;
                }
            }

            if(you.step())
            {
                you.side ? you.sprite.setTexture(hub[2]) : you.sprite.setTexture(hub[4]);
            }

            if (timer%spawn==0)
            {
                enemies.push_back(Enemy(rand()%640, rand()%2*660-20, 0, hub[3]));
            }
            if (timer%(spawn*14)==0 && timer>=60*60)
            {
                enemies2.push_back(Enemy(rand()%640, rand()%2*660-20, 0, hub[13]));
            }
            if (timer%(spawn*12)==0 && timer>=180*60)
            {
                enemies.push_back(Enemy(rand()%(640-140), rand()%2*660-20, 1, hub[34]));
            }
            if (timer%64==0)
            {
                guys.push_back(Guy(64+timer, hub[1]));
            }
            if (spawn>10 && ((spawn>30 && timer%(6*60)==0) || (timer%(11*60)==0)))
            {
                spawn--;
                if (spawn==10)
                    cout << "MAX - ";
                //cout << "NOWY SPAWN TIME: " << spawn << "\n";
                //cout << anims.size() << " " << anims2.size() << " " << bullets.size() << "\n";
            }
            if (guys.size()==0 || (guys.size()==1 && guys[0].x<0))
            {
                tryb=2;
            }
        }
    }

    void mouse(sf::Vector2i m)
    {
        if (tryb==1)
        {
            if (m.x>=256 && m.y>=479 && m.x<=383 && m.y<=582)
            {
                SPAWN=easy;
                tryb=4;
            }

            if (m.x>=256 && m.y>=552 && m.x<=383 && m.y<=615)
            {
                SPAWN=hard;
                tryb=4;
            }
        }
        else if (tryb==2)
        {
            tryb=1;
            //restart();
        }
        else if (tryb==4)
        {
            tryb=0;
            restart();
        }
        else if (buying==-1)
        {
            if (buy1.getGlobalBounds().contains(m.x, m.y) && score>=c1)
                buying=0;
            if (buy2.getGlobalBounds().contains(m.x, m.y) && score>=c2)
                buying=1;
            if (buy3.getGlobalBounds().contains(m.x, m.y) && score>=c3)
                buying=2;
            if (buy4.getGlobalBounds().contains(m.x, m.y) && score>=c4)
                buying=3;
        }
        else
        {
            switch (buying)
            {
                case 0:
                    towers.push_back(Tower(m.x, m.y, 125, 128, hub[5]));
                    score-=c1;
                    break;
                case 1:
                    towers.push_back(Tower(m.x, m.y, 60, 160, hub[18]));
                    score-=c2;
                    break;
                case 2:
                    towersM.push_back(Tower(m.x, m.y, 60, 196, hub[19]));
                    score-=c3;
                    break;
                case 3:
                    statue.setPosition(m.x, m.y);
                    tryb=3;
                    for (int i=0; i<guys.size(); i++)
                        guys[i].sprite.setTexture(hub[27]);
                    score-=c4;
                    break;
            }
            sScore.setString("fresh sacrifices: " + to_str(score));
            buying=-1;
        }
    }

    void draw(sf::RenderWindow &app)
    {
        if (tryb==0 || tryb==3 || tryb==2)
        {
            app.draw(bcg);
            for (int i=0; i<towers.size(); i++)
                towers[i].draw(app);
            for (int i=0; i<towersM.size(); i++)
                towersM[i].draw(app);
            for (int i=0; i<guys.size(); i++)
                guys[i].draw(app);
            for (int i=0; i<enemies.size(); i++)
                enemies[i].draw(app);
            for (int i=0; i<enemies2.size(); i++)
                enemies2[i].draw(app);
            for (int i=0; i<bullets.size(); i++)
                bullets[i].draw(app);
            for (int i=0; i<lines.size(); i++)
                lines[i].draw(app);
            for (int i=0; i<anims.size(); i++)
                anims[i].draw(app);
            app.draw(fire);

            you.draw(app);
            for (int i=0; i<anims2.size(); i++)
                anims2[i].draw(app);
        }
        if (tryb==0)
        {
            if (buying!=-1)
            {
                pointer.setPosition(sf::Vector2f(sf::Mouse::getPosition(app)));
                app.draw(pointer);
            }
            app.draw(sScore);
            app.draw(buy1);
            app.draw(buy2);
            app.draw(buy3);
            app.draw(buy4);
            if (score<c1)
                app.draw(locked1);
            if (score<c2)
                app.draw(locked2);
            if (score<c3)
                app.draw(locked3);
            if (score<c4)
                app.draw(locked4);
            app.draw(sc1);
            app.draw(sc2);
            app.draw(sc3);
            app.draw(sc4);
        }
        else if (tryb==2)
        {
            app.draw(gameover);
        }
        else if (tryb==3)
        {
            app.draw(statue);
            app.draw(youwon);
        }
        else if (tryb==1)
        {
            app.draw(menu);
        }
        else if (tryb==4)
        {
            app.draw(tut);
        }
    }
};

int main()
{
    srand(time(NULL));
    vector<sf::Texture> hub;
    hub.resize(41);
    hub[0].loadFromFile("img/bcg.png");
    hub[1].loadFromFile("img/guyproto.png");
    hub[2].loadFromFile("img/you.png");
    hub[3].loadFromFile("img/enemy.png");
    hub[4].loadFromFile("img/youm.png");
    hub[5].loadFromFile("img/tower.png");
    hub[6].loadFromFile("img/enemym.png");
    hub[7].loadFromFile("img/blood1.png");
    hub[8].loadFromFile("img/blood2.png");
    hub[9].loadFromFile("img/blood3.png");
    hub[10].loadFromFile("img/blood4.png");
    hub[11].loadFromFile("img/fire.png");
    hub[12].loadFromFile("img/gameover.png");
    hub[13].loadFromFile("img/enemy2.png");
    hub[14].loadFromFile("img/enemy2m.png");
    hub[15].loadFromFile("img/buy1.png");
    hub[16].loadFromFile("img/buy2.png");
    hub[17].loadFromFile("img/buy3.png");
    hub[18].loadFromFile("img/tower2.png");
    hub[19].loadFromFile("img/tower3.png");
    hub[20].loadFromFile("img/bullet.png");
    hub[21].loadFromFile("img/magic1.png");
    hub[22].loadFromFile("img/magic2.png");
    hub[23].loadFromFile("img/magic3.png");
    hub[24].loadFromFile("img/magic4.png");
    hub[25].loadFromFile("img/statue.png");
    hub[26].loadFromFile("img/buy4.png");
    hub[27].loadFromFile("img/guysmile.png");
    hub[28].loadFromFile("img/glow1.png");
    hub[29].loadFromFile("img/glow2.png");
    hub[30].loadFromFile("img/glow3.png");
    hub[31].loadFromFile("img/glow4.png");
    hub[32].loadFromFile("img/youglow.png");
    hub[33].loadFromFile("img/youglowm.png");
    hub[34].loadFromFile("img/enemy3.png");
    hub[35].loadFromFile("img/enemy3m.png");
    hub[36].loadFromFile("img/locked.png");
    hub[37].loadFromFile("img/pointer.png");
    hub[38].loadFromFile("img/menu.png");
    hub[39].loadFromFile("img/youwon.png");
    hub[40].loadFromFile("img/tut.png");

    sf::Music music;
    music.openFromFile("sounds/ost.wav");
    music.setLoop(true);
    music.play();

    Game game(hub);

    //sf::ContextSettings conset;
    //conset.antialiasingLevel = 4;
    sf::RenderWindow app(sf::VideoMode(640, 640), "ChosenProtector");//, sf::Style::Default, conset);
    app.setFramerateLimit(60);

    while (app.isOpen())
    {
        sf::Event event;
        while (app.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                app.close();

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Right)
                    game.buying=-1;
                game.mouse(sf::Mouse::getPosition(app));
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::M)
            {
                game.unmuted = !game.unmuted;
                if (!game.unmuted)
                    music.pause();
                else
                    music.play();
            }
        }

        if(game.you.cooldown==0)
        {
            if (game.you.y>0+16 && (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)))
            {
                game.you.y-=2;
                game.you.sprite.move(0,-2);
            }
            if (game.you.x>0+16 && (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)))
            {
                game.you.x-=2;
                game.you.sprite.move(-2,0);
                game.you.sprite.setTexture(hub[4]);
                game.you.side=0;
            }
            if (game.you.y<640-16 && (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)))
            {
                game.you.y+=2;
                game.you.sprite.move(0,2);
            }
            if (game.you.x<640-16+2 && (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)))
            {
                game.you.x+=2;
                game.you.sprite.move(2,0);
                game.you.sprite.setTexture(hub[2]);
                game.you.side=1;
            }
        }

        game.step();

        app.clear();
        game.draw(app);
        app.display();
    }

    return EXIT_SUCCESS;
}
