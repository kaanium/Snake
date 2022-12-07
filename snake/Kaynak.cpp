#include <iostream>
#include <time.h>
#include <conio.h> // konsol input output
#include <windows.h>
#include <fstream>
#include <vector>

#pragma comment(                                                          \
    lib, "winmm.lib") // yine bir cesit library ama include la yazilmiyor

enum yonler {
    UP = 'w',
    DOWN = 's',
    LEFT = 'a',
    RIGHT = 'd'
}; // okunulabilirlik icin

constexpr auto GRID = 30; // sahanin boyutunu kolayca degistirebilmek icin

void HideCursor(bool showFlag) // cursor cirkin gozukdugunden gozukmesin
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag;
    SetConsoleCursorInfo(out, &cursorInfo);
}

void ScreenReset(int x, int y) // konsol da alt alta, yan yana yazmak
                               // yerine benim dedigim yere bas
{
    static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    std::cout.flush();
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hOut, coord);
}

void Food(int& foodX, int& foodY, int size, std::vector<int>& randomx,
          std::vector<int>& randomy, bool Condition3)
{
    if (!(Condition3))
        return; // bug fix
    srand((unsigned int)time(NULL));
    int boy = randomx.size(), x1;
    if (size == 3) // ilk tur tarama olmadigindan rastgele bir yer sec
    {
        foodX = rand() % (GRID - 4) +
                3; // yilanin ustunde yemek koyma olasiligi var ama dusuk
                   // oldugundan yoksayiyorum
        foodY = rand() % (GRID - 4) + 3;
    }
    else // sadece bos alanlari sec
    {
        x1 = rand() % boy;
        x1 *= rand();
        x1 %= boy; // rand yeterince rastgele olmadigindan biraz islem yap
        foodX = randomx[x1], foodY = randomy[x1];
    }
}

void Eat(bool& Condition4, std::vector<int>& kuyrukx,
         std::vector<int>& kuyruky)
{
    Condition4 = false; // yeniden yemek olusturabilmek icin yemegin
                        // yenildigini bildir
    kuyrukx.push_back(0);
    kuyruky.push_back(0); // yemek yediginde buyu verilen degerin onemi yok
    mciSendString(
        L"close C:\\Users\\KAAN\\source\\repos\\snake\\chomp.mp3", NULL, 0,
        NULL); // bug fix
    mciSendString(L"open C:\\Users\\KAAN\\source\\repos\\snake\\chomp.mp3",
                  NULL, 0, NULL);
    mciSendString(
        TEXT("setaudio C:\\Users\\KAAN\\source\\repos\\snake\\chomp.mp3 "
             "volume to 10"),
        NULL, 0, NULL); // sesi kis
    mciSendString(L"play C:\\Users\\KAAN\\source\\repos\\snake\\chomp.mp3",
                  NULL, 0, NULL); // yemek yeme sesi
}

void Movement(int& snakeX, int& snakeY, int& mem, int foodx, int foody,
              bool& Condition4, std::vector<int>& kuyrukx,
              std::vector<int>& kuyruky, bool& Condition5,
              bool& Condition2)
{
    static int memX = 0,
               memY =
                   1; // onceki gidilen yolu hatirlayabilmek icin statics
    if (Condition2)
        memX = 0, memY = 1,
        Condition2 = false; // olduyse saga hareketi yenile
    int c = 0;
    clock_t start = clock();
    while (!_kbhit()) {
        if ((((double)clock() - start) / CLOCKS_PER_SEC) >=
            0.15) // hareket hizi
        {
        Continue:;
            snakeX += memX; // tusa basilmazsa onceki turun yonunu uygula
            snakeY += memY;
            if (snakeX == GRID - 1)
                snakeX = 1; // duvarlarin arasýndan gecme
            else if (snakeX == 0)
                snakeX = GRID - 2;
            else if (snakeY == GRID - 1)
                snakeY = 1;
            else if (snakeY == 0)
                snakeY = GRID - 2;
            if (snakeX == foodx && snakeY == foody) {
                Eat(Condition4, kuyrukx, kuyruky);
                Condition5 = true; // bug fix
            }
            return;
        }
    }
    switch ((c = _getch())) {
    case UP:
        if (mem == DOWN) {
            goto Continue; // zit yone gitmeye calisirsa onceki yonde devam
                           // et
        }
        --snakeX;
        if (snakeX == 0)
            snakeX = GRID - 2; // duvarlarýn arasýndan gecme
        memX = -1;             //
        memY = 0; // tusa basilmazsa diye bu tur yapilanlari kullanabilmek
                  // icin kaydet
        mem = UP; //
        if (snakeX == foodx && snakeY == foody) {
            Eat(Condition4, kuyrukx, kuyruky);
            Condition5 = true;
        }
        break;
    case DOWN:
        if (mem == UP) {
            goto Continue;
        }
        ++snakeX;
        if (snakeX == GRID - 1)
            snakeX = 1;
        memX = 1;
        memY = 0;
        mem = DOWN;
        if (snakeX == foodx && snakeY == foody) {
            Eat(Condition4, kuyrukx, kuyruky);
            Condition5 = true;
        }
        break;
    case LEFT:
        if (mem == RIGHT) {
            goto Continue;
        }
        --snakeY;
        if (snakeY == 0)
            snakeY = GRID - 2;
        memY = -1;
        memX = 0;
        mem = LEFT;
        if (snakeX == foodx && snakeY == foody) {
            Eat(Condition4, kuyrukx, kuyruky);
            Condition5 = true;
        }
        break;
    case RIGHT:
        if (mem == LEFT) {
            goto Continue;
        }
        ++snakeY;
        if (snakeY == GRID - 1)
            snakeY = 1;
        memY = 1;
        memX = 0;
        mem = RIGHT;
        if (snakeX == foodx && snakeY == foody) {
            Eat(Condition4, kuyrukx, kuyruky);
            Condition5 = true;
        }
        break;
    default:
        exit(0); // programi kapat
    }
}

void Snake(std::vector<int>& kuyrukx, std::vector<int>& kuyruky,
           int snakex, int snakey)
{
    for (int i = kuyrukx.size();
         --i;) // kuyrugu soldan baslayarak sagdan sola dogru tasi
    {
        kuyrukx[i] =
            kuyrukx[i - 1]; // 12345 -> 22345 -> 23345 -> 23445 -> 2345x
        kuyruky[i] = kuyruky[i - 1];
    }
    kuyrukx[0] = snakex; // for dongusu 0 i atladigindan disarida
    kuyruky[0] = snakey;
}

int main()
{
    HANDLE hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleDisplayMode(hConsole, CONSOLE_FULLSCREEN_MODE,
                          0); // tam ekran yap
    mciSendString(L"open C:\\Users\\KAAN\\source\\repos\\snake\\son1.mp3",
                  NULL, 0, NULL);
    mciSendString(
        TEXT("setaudio C:\\Users\\KAAN\\source\\repos\\snake\\son1.mp3 "
             "volume to 30"),
        NULL, 0, NULL);
    mciSendString(
        L"play C:\\Users\\KAAN\\source\\repos\\snake\\son1.mp3 repeat",
        NULL, 0, NULL); // muzik
    std::ios_base::sync_with_stdio(false);
    std::cout.tie(NULL); // daha hizli cout
    bool Condition2 = false, Condition3 = false;
    int rekor, size, headmemx, headmemy, foodx = 6, foody = 7, memX1,
                                         memY1, memX2, memY2, sonkuyrukX,
                                         sonkuyrukY, Condition1;
    std::fstream highScore(
        "Rekor.txt",
        std::ios::in | std::ios::out |
            std::ios::app); // hem input hem output yapilabilsin
    highScore >> rekor;     // dosyadan kayitli rekoru al
    highScore.close();
    std::vector<int> kuyrukx, kuyruky, randomx,
        randomy; // x ve y yerine iki boyutlu dizi mi yapsaydým?
restart:         // olunce bastan basla
    Condition1 = 1, memX1 = GRID / 2 - 1, memY1 = GRID / 2,
    memX2 = GRID / 2 - 3,
    memY2 = GRID / 2;          // yeniden baslandigindan degerleri sifirla
    size = kuyrukx.size() - 3; // direk kendisini koyunca sorun oldu?
                               // ayrýca iceridede kullandigimdan boyutunu
                               // iki kere hesaplamaya gerek kalmasin diye
    if (size > rekor) // skor rekoru gectiyse dosyaya yeni rekoru kaydet
    {
        rekor = size;
        highScore.open(
            "Rekor.txt",
            std::ofstream::out |
                std::ofstream::trunc); // trunc = dosyanin icini temizle
        highScore << rekor;
        highScore.close();
    }
    kuyrukx.clear(),
        kuyruky.clear(); // yeniden baslandiysa onceki kuyrugu sil
    int snakex = GRID / 2, snakey = GRID / 2,
        mem = RIGHT;                      // baslangic konumu ve yonu
    SetConsoleTextAttribute(hConsole, 7); // renk ayarlama fonksiyonu
    ScreenReset(0, 0); // yazi yazan cursorun yerini sol ustle degistir ki
                       // yeniden baslayinca konsol ekrani temizlensin
    std::cout
        << "                                        "; // bug fix ilk
                                                       // satiri temizle
    HideCursor(false);                                 // cursor gozukmesin
    kuyrukx.push_back(GRID / 2), kuyruky.push_back(GRID / 2 - 1),
        kuyrukx.push_back(GRID / 2);
    kuyruky.push_back(GRID / 2 - 2), kuyrukx.push_back(GRID / 2),
        kuyruky.push_back(GRID / 2 - 3); // ilk 3 kuyrugu olustur
    sonkuyrukY = kuyruky.back(),
    sonkuyrukX = kuyrukx.back(); // kuyrugun ucunu kaydet
    bool Condition4 = false, Condition5 = false;
    ScreenReset(0, 0);
    for (int p = GRID / 3 + 2; p--;)
        std::cout << "  ";
    std::cout << "Puan: " << kuyrukx.size() - 3 << '\n';
    for (int p = GRID / 3 + 2; p--;)
        std::cout << "  ";
    std::cout << "Rekor: " << rekor << "\n\n";
    Condition3 = true;
    int SIZE = kuyrukx.size(); // diziyi fonksiyona yollamaktansa int
                               // yollamak daha verimli
    Food(foodx, foody, SIZE, randomx, randomy,
         Condition3); // bug fix foodu bir kereligine dongu disarida
                      // calistir
    Condition3 = false;
    for (int i = 0; i < GRID; ++i) // sahayi ciz
    {
        for (int j = 0; j < GRID; ++j) {
            if (i == 0 || i == GRID - 1 || j == 0 || j == GRID - 1) {
                SetConsoleTextAttribute(hConsole, 64);
                std::cout << "# "; // sahayi kare yapabilmek icin sutunu
                                   // ikiyle carpmak lazim bu yuzden de iki
                                   // karakter basiyoruz
            }
            else if (i == foodx && foody == j) {
                SetConsoleTextAttribute(hConsole, 79);
                std::cout << "X ";
            }
            else if (i == snakex && snakey == j) {
                SetConsoleTextAttribute(hConsole, 74);
                std::cout << "O ";
            }
            else {
                SetConsoleTextAttribute(hConsole, 74);
                for (int l = kuyrukx.size(), ll = kuyrukx.size(); l--;) {
                    if (kuyrukx[l] == i &&
                        kuyruky[l] == j) // kuyruk cizdirme
                    {
                        if (l != ll - 1)
                            std::cout << "o "; // kuyrugun ucu degilse 'o'
                                               // ucuysa '+' bas
                        else
                            std::cout << "+ ";
                        break;
                    }
                    else if (!(l))
                        std::cout
                            << "  "; // kuyruk bulamadiysan bos alan bas
                }
            }
        }
        std::cout << '\n';
    }
    while (true) {
        headmemx = snakex;
        headmemy = snakey; // su anki kafa konumu
        Movement(snakex, snakey, mem, foodx, foody, Condition4, kuyrukx,
                 kuyruky, Condition5, Condition2);
        for (int column = 1; column < GRID - 1; ++column)
            // bos kareleri tarayip kaydet bunlar yemegin rastgele
            // olusturulurken sadece bos kare de spawn olmasini saglayacak
            for (int row = 1; row < GRID - 1; ++row) {
                if ((column == snakex && row == snakey) ||
                    headmemx == column && headmemy == row)
                    continue;
                for (int co = kuyrukx.size(); co--;) {
                    if (column == kuyrukx[co] && row == kuyruky[co])
                        break;
                    else if (!(co))
                        randomx.push_back(column), randomy.push_back(row);
                }
            }
        SIZE = kuyrukx.size();
        if (!(Condition4))
            Food(foodx, foody, SIZE, randomx, randomy,
                 Condition3); // yemek yenilmisse yeniden yemek olustur
        Condition3 =
            true; // food u while dongusu icinde calisilibalir hale getir
        randomx.clear(),
            randomy.clear(); // bos kareleri yeniden tarayabilmek icin
                             // diziyi temizle
        for (int l = kuyrukx.size() - 1;
             l--;) // olum, -1 cunku ilk kafanin pozisyonunu
                   // hesapladigindan kuyrugun ucuna degdigi olasiligi
                   // yoksayiyorum
        {
            if (kuyrukx[l] == snakex &&
                kuyruky[l] == snakey) // kafa kuyruga degdiyse ol
            {
                mciSendString(
                    L"play "
                    L"C:\\Users\\KAAN\\source\\repos\\snake\\roblox.mp3",
                    NULL, 0, NULL); // olum sesi
                Condition2 = true;
                goto restart;
            }
        }
        Snake(kuyrukx, kuyruky, headmemx, headmemy);
        Condition4 = true;
        ScreenReset(0, 0);
        SetConsoleTextAttribute(hConsole, 7);
        for (int p = GRID / 3 + 2; p--;)
            std::cout << "  ";
        std::cout << "Puan: " << kuyrukx.size() - 3 << '\n';
        for (int p = GRID / 3 + 2; p--;)
            std::cout << "  ";
        std::cout << "Rekor: " << rekor << "\n\n";
        ScreenReset(snakey * 2,
                    snakex +
                        3); // !sahayi her tur bastan cizmek yerine sadece
                            // hareket edenleri cizip digerlerini sabit tut
        SetConsoleTextAttribute(hConsole, 74);
        std::cout << "O ";
        ScreenReset(memX1 * 2,
                    memY1 +
                        3); // iki bosluk kullanildigindan 2 ile carpiyoruz
        std::cout << "o ";
        ScreenReset(kuyruky.back() * 2,
                    kuyrukx.back() + 3); // saha 3. satýrdan sonra basliyor
        std::cout << "+ ";
        ScreenReset(foody * 2, foodx + 3);
        SetConsoleTextAttribute(hConsole, 79);
        std::cout << "X ";
        if (!((foody * 2 == memX2 && foodx == memY2) ||
              (snakey * 2 == memX2 && snakex == memY2))) // bug fixes
        {
            ScreenReset(memX2, memY2 + 3);
            std::cout << "  ";
        }
        if (Condition1 == 1) {
            ScreenReset(GRID - 1, GRID / 2 + 3);
            SetConsoleTextAttribute(hConsole, 74);
            std::cout << "o ";
            ScreenReset(sonkuyrukY * 2, sonkuyrukX + 3);
            std::cout << "  ";
        }
        if (Condition5) {
            ScreenReset(sonkuyrukY * 2, sonkuyrukX + 3);
            SetConsoleTextAttribute(hConsole, 74);
            std::cout << "+ ";
            Condition5 = false;
        }
        // sonraki tur yazdirirken kullanmak icin suan ki konumlari kaydet
        memX1 = snakey, memY1 = snakex, memX2 = kuyruky.back() * 2,
        memY2 = kuyrukx.back(), ++Condition1;
        sonkuyrukY = kuyruky.back(), sonkuyrukX = kuyrukx.back();
    }
}