//
//
// Your code starts on line 2406. (as of 10.17.2020 at 4:27 pm)
//
//
// You may modify the code after the #OFFICIAL_LINE found near there.
// (you can also just do a text search for #OFFICIAL_LINE ...)
//
//
// instructions: Refer to the Assignment 3 PDF and
//               complete as many of the tasks as you can.
//
//               I'd recommend completing tasks 1.1 and 1.2 first.
//
//               If the mkdir function in the main method give you
//               problems on your machine, you may delete it.
//               don't mess around any of the other lines.
//
// Good Luck!
//




// ------------ Start of Bryce's Code, do not modify ----------------
//
// ITP - 132, C++ Programming
// Assignment 3, Image Processor.
// First Released by Bryce Summers on 10.17.2020.
//
// Note: Tested on Online GDB in C++ mode.
//       On my machine the code produces a new io folder and logs
//       that open as additional windows in Online GDB.
//
//       On a desktop, you should see the "./io" folder created.
//       Testing results will be stored in files there.
//

#include <iostream>
#include <iomanip>

#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include <sys/stat.h> // Creating IO folder.

using namespace std;

// Definitions for types used.
#define Image  vector<char>
#define Bounds vector<float>


// Integer and Char constants that you may wish to make use of.
const int ERROR_DIMENSION_MISMATCH = -1;
const int ERROR_EMPTY_IMAGE = -2;

const char V_IN   = '@';
const char V_OUT  = '.';

const char V_ODD  = ',';//'`';
const char V_EVEN = '.'; // .
const char V_PARTIAL = ' ';

const char V_TRUE   = '@';
const char V_FALSE  = '.';

//const int MAX_ITERS = 256;
const int MAX_ITERS = 100;

const char V_PARTIAL_0 = ' ';
const char V_PARTIAL_1 = '.';
const char V_PARTIAL_2 = ',';
const char V_PARTIAL_3 = 'o';
const char V_PARTIAL_4 = '@';


// ====> Part A function declarations. <====

// -- Validation Functions
bool validateImage(Image & image);
bool checkBounds(Image & image, int row, int col);

// -- Encoding Functions.
bool decode_image_bif_01(istream & in, Image & out);
bool encode_image_bif_01(Image & in, ostream & out);

bool decode_image_bif_02(istream & in, Image & out);
bool encode_image_bif_02(Image & in, ostream & out);

bool decode_image_bif_03(istream & in, Image & out);
bool encode_image_bif_03(Image & in, ostream & out);

bool decode_image_bif_04(istream & in, Image & out);
bool encode_image_bif_04(Image & in, ostream & out);



// ====> Part B function declarations. <====

// -- Mandelbrot Set Image Generation Functions.
bool escaped(float z_x, float z_y);
void iterate(float z_x, float z_y,
             float c_x, float c_y,
             float & x_out, float & y_out);
int  mandelbrotEscapeTime(float c_x, float c_y, int give_up);
char sampleFromMandelbrot(float c_x, float c_y);
float lerp(float val1, float val2, float time_from_0_to_1);

// Fills in the given valid image with characters sampled evenly
// from starting points on the given rectangle:
//
//   (x1, y1)     row 0     (x2, y1)
//           +-------------+
//           |             |               |
//  column 0 |             | last column.  | new line column
//           |             |               | (last column + 1)
// last row  +-------------+
//   (x1, y2)   last row    (x2, y2)
// The image already has well defined row and column dimensions.
bool render(Image & blank_image_of_desired_size, Bounds bounds,
            char (*pointSampler)(float c_x, float c_y));

// Wrapper function (Task 3.7)
bool render(Image & image, int rows, int cols,
            char (*sampler)(float, float));

char superSampleMandelbrot(float c_x, float c_y, float region_w, float region_h);

// Samples or super samples rectangular regions centered at cx, cy
// with the give width and height.
// The top left corner is at point     (cx - width/2, cy - height/2)
// The bottom right corner is at point (cx + width/2, cy + height/2)
bool render(Image & blank_image_of_desired_size, Bounds bounds,
            char (*regionSampler)(float c_x,   float c_y,
                                  float width, float height));

// Wrapper function. (Task 3.10)
bool render(Image & image, int rows, int cols,
            char (* sampleRegion)(float, float, float, float));



// ====> Part C function declarations. <====

Image operator ! (const Image right);                   // Task 4.1
Image operator & (const Image left, const Image right); // Task 4.2
Image operator | (const Image left, const Image right); // Task 4.3
Image operator ^ (const Image left, const Image right); // Task 4.4

//=================================================================



// Pre-rendered images used as test data and output goals.

const string OFFICIAL_TEST_DATA_1 =
    "      ,,o@@@@@@;,,   ---\\\\B   \n"
    "   .o@@@@@@@@@@@@@@;.   \\\\y   \n"
    "  o@@@@@@@@@@@@@@@@@@;  \\\\    \n"
    " o@@@@@@@@@@@@@@@@@@@@;  .B S \n"
    ";@@@@@@@@@@@@@@@@@@@@@@; .r u \n"
    "@@@@@@@@@@@@@@@@@@@@@@@@ .y m \n"
    "@ ITP - 132 Test Image @ .c m \n"
     "@@@@@@@@@@@@@@@@@@@@@@@@ .e e \n"
    ";@@@@        @      @@@; .  r \n"
    " o@@@@@@@ @@@@@@ @@@@@?  .  s \n"
    "  o@@@@@@ @@@@@@@ @@@?   .    \n"
    "   `o@@@@ @@@@@@@ @?`   |.on  \n"
    "      ^^o@@@@@@?^^      |.    \n"
    "A       ,,o@@@@@@;,,   | .1   \n"
    "s    .o@@@@\\\\@\\@@\\@@@;.|  0   \n"
    "s   o@@@@@@@@@@@@@@@@@@|  .   \n"
    "i  o@@@@@@@@^@\\@@@^@@@@|; 1   \n"
    "g ;@@@@@@    @@@   @@Q@@|;6   \n"
    "n @@@@@@@@@@@@ @@@@@@@/|/ .   \n"
    "m @@@@@O@@O@O@@ @@O@@/O/ @2   \n"
    "e ;@@@@@@@@@#@@#@@#@@|/ @;0   \n"
    "n  o@@@@@@@^^^o^^^^@/_ @? 2   \n"
    "t   o@@@@@@@@\\ ,@@@/   ?  0   \n"
    "     `o@@@@@@@@@@@@@          \n"
    "3       ^^o@@@@@@?^^          \n";


const string OFFICIAL_TEST_DATA_2 =
    "                                                                                  \n"
    "                                   ,,d@@@@@@;,,                                   \n"
    "                                 d@@@@@@@@@@@@@@;,                                \n"
    "                               d@@@@@@@@**@@@@@@@@;                               \n"
    "                              d@@@@@*`      `*@@@@@;                              \n"
    "       ,,d@@@@@@;,,          `@@@@@`  TTTTTT  `@@@@@;         ,,d@@@@@@;,,        \n"
    "     d@@@@@@@@@@@@@@;,       @@@@@;     TT     `@@@@@       d@@@@@@@@@@@@@@;,     \n"
    "   d@@@@@@@@**@@@@@@@@;      @@@@@;     TT     `@@@@@     d@@@@@@@@**@@@@@@@@;    \n"
    "  d@@@@@*`      `*@@@@@;     `@@@@@,    TT     @@@@@;    d@@@@@*`      `*@@@@@;   \n"
    " `@@@@@`  IIIIII  `@@@@@;     *@@@@@,,       ,@@@@@@    `@@@@@`   PPPPP  `@@@@@;  \n"
    " @@@@@;     II     `@@@@@      *@@@@@@@@,,@@@@@@@@@     @@@@@;    P    P  `@@@@@  \n"
    " @@@@@;     II     `@@@@@       `*@@@@@@@@@@@@@@@`      @@@@@;    PPPPP   `@@@@@  \n"
    " `@@@@@,  IIIIII    @@@@@;          ***@@@@@@@**         `@@@@@,  P        @@@@@; \n"
    "  *@@@@@,,       ,@@@@@@                                 *@@@@@,, P     ,@@@@@@   \n"
    "   *@@@@@@@@,,@@@@@@@@@                                   *@@@@@@@@,,@@@@@@@@@    \n"
    "    `*@@@@@@@@@@@@@@@`        ITP - 132, Assignment 3      `*@@@@@@@@@@@@@@@`     \n"
    "       ***@@@@@@@**        By Bryce Summers, 10.16.2020       ***@@@@@@@**        \n"
    "                                                                                  \n";


// Points means point sampled, regions means super sampled 4x.
// BROT_rows_columns_SAMPLING_METHOD.
const string OFFICIAL_BROT_25_69_POINTS =
    ",,,,,,.....,,,,,,,,,,,,,,,,,,........,,,,......,.....,,,,,...........\n"
    ",,,,,....,,,,,,,,,,,,,,,,,,........,,,...,.@.,..,,.....,,,,,.........\n"
    ",,,,...,,,,,,,,,,,,,,,,,,........,,...,.,@@@@,.,.,,,,...,,,,,,.......\n"
    ",,,..,,,,,,,,,,,,,,,,,........,.,,,,,.,..@@@@,...,....,..,,,,,,,.....\n"
    ",,..,,,,,,,,,,,,,,,,.....,,,,......,,@.@@@@@@@,.,,,.,..,,.,,,,,,,....\n"
    ",,.,,,,,,,,,,,,,,,...,,,,,,,..,..@@,@@@@@@@@@@@@@@,,@@,.,,.,,,,,,,...\n"
    ",.,,,,,,,,,,,,...,,,,,,,,.....,,,@@@@@@@@@@@@@@@@@@@..,,.,..,,,,,,,..\n"
    ",.,,,,,,,.....,...,,,,,,,,,,..,@@@@@@@@@@@@@@@@@@@@@@@.,.,..,,,,,,,..\n"
    ",,,,,......,,,.......,...,,,..,@@@@@@@@@@@@@@@@@@@@@@@@,,....,,,,,,,.\n"
    ".,.......,,,,,.,...@.@@@@,,.,@@@@@@@@@@@@@@@@@@@@@@@@@@..,...,,,,,,,.\n"
    ",.......,,,,.,.,,,,@@@@@@@@@,@@@@@@@@@@@@@@@@@@@@@@@@@@,.,...,,,,,,,,\n"
    "..,,.,....,,.,.,.,@@@@@@@@@@,@@@@@@@@@@@@@@@@@@@@@@@@@,,.,....,,,,,,,\n"
    "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@....,,....,,,,,,,\n"
    "..,,.,....,,.,.,.,@@@@@@@@@@,@@@@@@@@@@@@@@@@@@@@@@@@@,,.,....,,,,,,,\n"
    ",.......,,,,.,.,,,,@@@@@@@@@,@@@@@@@@@@@@@@@@@@@@@@@@@@,.,...,,,,,,,,\n"
    ".,.......,,,,,.,...@.@@@@,,.,@@@@@@@@@@@@@@@@@@@@@@@@@@..,...,,,,,,,.\n"
    ",,,,,......,,,.......,...,,,..,@@@@@@@@@@@@@@@@@@@@@@@@,,....,,,,,,,.\n"
    ",.,,,,,,,.....,...,,,,,,,,,,..,@@@@@@@@@@@@@@@@@@@@@@@.,.,..,,,,,,,..\n"
    ",.,,,,,,,,,,,,...,,,,,,,,.....,,,@@@@@@@@@@@@@@@@@@@..,,.,..,,,,,,,..\n"
    ",,.,,,,,,,,,,,,,,,...,,,,,,,..,..@@,@@@@@@@@@@@@@@,,@@,.,,.,,,,,,,...\n"
    ",,..,,,,,,,,,,,,,,,,.....,,,,......,,@.@@@@@@@,.,,,.,..,,.,,,,,,,....\n"
    ",,,..,,,,,,,,,,,,,,,,,........,.,,,,,.,..@@@@,...,....,..,,,,,,,.....\n"
    ",,,,...,,,,,,,,,,,,,,,,,,........,,...,.,@@@@,.,.,,,,...,,,,,,.......\n"
    ",,,,,....,,,,,,,,,,,,,,,,,,........,,,...,.@.,..,,.....,,,,,.........\n"
    ",,,,,,.....,,,,,,,,,,,,,,,,,,........,,,,......,.....,,,,,...........\n";


const string OFFICIAL_BROT_80_30_POINTS =
    ",,,..,,,,,,,,...,.,.,..,,.....\n"
    ",,...,,,,,,,,...,...,..,,.....\n"
    ",,..,,,,,,,,....,,.,...,,,....\n"
    ",,..,,,,,,,,...,,,,.....,,....\n"
    ",,..,,,,,,,,...,,.,..,..,,....\n"
    ",,..,,,,,,,....,.,,,,,..,,....\n"
    ",,.,,,,,,,,....,.,@,.,,.,,,...\n"
    ",,.,,,,,,,,...,.,,@..,,.,,,...\n"
    ",,.,,,,,,,,...,.,.@@.,,..,,...\n"
    ",,.,,,,,,,......,.@@..,,.,,...\n"
    ",.,,,,,,,,...,.,.,@@...,.,,,..\n"
    ",.,,,,,,,,...,,,,,@@,,...,,,..\n"
    ",.,,,,,,,...,.....@@.,...,,,..\n"
    ",.,,,,,,,...,,.,..@,.,,.,,,,..\n"
    ",.,,,,,,,..,,,.,.@@@@,.,,,,,..\n"
    ",.,,,,,,..,,.,,.@@@@@,,,,,,,..\n"
    ",.,,,,,,..,,.,..@@@@@@.,,.,,..\n"
    ",,,,,,,,.,,,...,@@@@@@@,,.,,,.\n"
    ",,,,,,,..,,,..,@@@@@@@@.,.,,,.\n"
    ",,,,,,,.,,,..,,@@@@@@@@...,,,.\n"
    ",,,,,,.,,,,.,.,@@@@@@@@,..,,,.\n"
    ",,,,,,..,,,.,.@@@@@@@@@...,,,.\n"
    ".,,,,..,.,...@@@@@@@@@@,..,,,.\n"
    ".,,,..,,..,,.@@@@@@@@@@.,.,,,.\n"
    ".,,,...,,.,,..@@@@@@@@@.,.,,,.\n"
    ".,,..,..,..,,@@@@@@@@@@,,.,,,.\n"
    ",,...,.,..,..@@@@@@@@@@@,.,,,,\n"
    ",,...,.,,,,,.@@@@@@@@@@@..,,,,\n"
    ",,..,,.,.,,..@@@@@@@@@@@,..,,,\n"
    ",...,,.,@,...@@@@@@@@@@@...,,,\n"
    ",...,,,,@@@,.@@@@@@@@@@@...,,,\n"
    ",...,,,,,@@.,@@@@@@@@@@@...,,,\n"
    ",...,,,,@@@@,@@@@@@@@@@@,..,,,\n"
    "...,,..,@@@@,@@@@@@@@@@@,..,,,\n"
    "...,..,,@@@@.@@@@@@@@@@@...,,,\n"
    "......,.@@@@,@@@@@@@@@@@...,,,\n"
    "..,.,.,,@@@@,@@@@@@@@@@....,,,\n"
    ",.,.,,,.@@@@,@@@@@@@@@@,...,,,\n"
    ",.,..,@@@@@@,@@@@@@@@@@,...,,,\n"
    ",.....@@@@@@@@@@@@@@@@@....,,,\n"
    ",.....@@@@@@@@@@@@@@@@@....,,,\n"
    ",.,..,@@@@@@,@@@@@@@@@@,...,,,\n"
    ",.,.,,,.@@@@,@@@@@@@@@@,...,,,\n"
    "..,.,.,,@@@@,@@@@@@@@@@....,,,\n"
    "......,.@@@@,@@@@@@@@@@@...,,,\n"
    "...,..,,@@@@.@@@@@@@@@@@...,,,\n"
    "...,,..,@@@@,@@@@@@@@@@@,..,,,\n"
    ",...,,,,@@@@,@@@@@@@@@@@,..,,,\n"
    ",...,,,,,@@.,@@@@@@@@@@@...,,,\n"
    ",...,,,,@@@,.@@@@@@@@@@@...,,,\n"
    ",...,,.,@,...@@@@@@@@@@@...,,,\n"
    ",,..,,.,.,,..@@@@@@@@@@@,..,,,\n"
    ",,...,.,,,,,.@@@@@@@@@@@..,,,,\n"
    ",,...,.,..,..@@@@@@@@@@@,.,,,,\n"
    ".,,..,..,..,,@@@@@@@@@@,,.,,,.\n"
    ".,,,...,,.,,..@@@@@@@@@.,.,,,.\n"
    ".,,,..,,..,,.@@@@@@@@@@.,.,,,.\n"
    ".,,,,..,.,...@@@@@@@@@@,..,,,.\n"
    ",,,,,,..,,,.,.@@@@@@@@@...,,,.\n"
    ",,,,,,.,,,,.,.,@@@@@@@@,..,,,.\n"
    ",,,,,,,.,,,..,,@@@@@@@@...,,,.\n"
    ",,,,,,,..,,,..,@@@@@@@@.,.,,,.\n"
    ",,,,,,,,.,,,...,@@@@@@@,,.,,,.\n"
    ",.,,,,,,..,,.,..@@@@@@.,,.,,..\n"
    ",.,,,,,,..,,.,,.@@@@@,,,,,,,..\n"
    ",.,,,,,,,..,,,.,.@@@@,.,,,,,..\n"
    ",.,,,,,,,...,,.,..@,.,,.,,,,..\n"
    ",.,,,,,,,...,.....@@.,...,,,..\n"
    ",.,,,,,,,,...,,,,,@@,,...,,,..\n"
    ",.,,,,,,,,...,.,.,@@...,.,,,..\n"
    ",,.,,,,,,,......,.@@..,,.,,...\n"
    ",,.,,,,,,,,...,.,.@@.,,..,,...\n"
    ",,.,,,,,,,,...,.,,@..,,.,,,...\n"
    ",,.,,,,,,,,....,.,@,.,,.,,,...\n"
    ",,..,,,,,,,....,.,,,,,..,,....\n"
    ",,..,,,,,,,,...,,.,..,..,,....\n"
    ",,..,,,,,,,,...,,,,.....,,....\n"
    ",,..,,,,,,,,....,,.,...,,,....\n"
    ",,...,,,,,,,,...,...,..,,.....\n"
    ",,,..,,,,,,,,...,.,.,..,,.....\n";


const string OFFICIAL_BROT_25_50_REGIONS =
    ",,,, .. ,,,,,,,,,,,,  .... ,        .. ,,, .......\n"
    ",,, .. ,,,,,,,,,,,, ..... ,        , .. ,,, ......\n"
    ",, .  ,,,,,,,,,,, .....   .   @@    ,, . ,,,  ....\n"
    ",,  ,,,,,,,,,,,, ...          @@     .    ,,,, ...\n"
    ", . ,,,,,,,,,, ..  ,,          @           ,,, ...\n"
    ",  ,,,,,,,,,    ,,,        @@@@@@@@@       ,,,, ..\n"
    ", ,,,,,,,     ,,,, .     @@@@@@@@@@@@@   ,. ,,,, .\n"
    "  ,,,   .          ,   @@@@@@@@@@@@@@@@   . ,,,, .\n"
    " ,, ...  ,            @@@@@@@@@@@@@@@@@   ..,,,,, \n"
    "  .... ,,,            @@@@@@@@@@@@@@@@@   .. ,,,, \n"
    " .... ,       @@@@@  @@@@@@@@@@@@@@@@@@   .. ,,,,,\n"
    "     .       @@@@@@@ @@@@@@@@@@@@@@@@@@  , . ,,,,,\n"
    ",      ,   @@@@@@@@@@@@@@@@@@@@@@@@@@@  ., . ,,,,,\n"
    "     .       @@@@@@@ @@@@@@@@@@@@@@@@@@  , . ,,,,,\n"
    " .... ,       @@@@@  @@@@@@@@@@@@@@@@@@   .. ,,,,,\n"
    "  .... ,,,            @@@@@@@@@@@@@@@@@   .. ,,,, \n"
    " ,, ...  ,            @@@@@@@@@@@@@@@@@   ..,,,,, \n"
    "  ,,,   .          ,   @@@@@@@@@@@@@@@@   . ,,,, .\n"
    ", ,,,,,,,     ,,,, .     @@@@@@@@@@@@@   ,. ,,,, .\n"
    ",  ,,,,,,,,,    ,,,        @@@@@@@@@       ,,,, ..\n"
    ", . ,,,,,,,,,, ..  ,,          @           ,,, ...\n"
    ",,  ,,,,,,,,,,,, ...          @@     .    ,,,, ...\n"
    ",, .  ,,,,,,,,,,, .....   .   @@    ,, . ,,,  ....\n"
    ",,, .. ,,,,,,,,,,,, ..... ,        , .. ,,, ......\n"
    ",,,, .. ,,,,,,,,,,,,  .... ,        .. ,,, .......\n";


const string OFFICIAL_BROT_41_40_REGIONS =
    ",,, .. ,,,,,,,,,, ... ,     .. ,, ......\n"
    ",,,.. ,,,,,,,,,, ... ,       .. ,, .....\n"
    ",, ..,,,,,,,,,, .... ,      , . ,,,.....\n"
    ",, . ,,,,,,,,, ....         ,, . ,, ....\n"
    ",,. ,,,,,,,,, ....  .   @@   ,,  ,,, ...\n"
    ",,. ,,,,,,,,, ...       @@   .  .,,, ...\n"
    ",  ,,,,,,,,, ..         @@        ,,, ..\n"
    ",  ,,,,,,,, .. ,,      @@@        ,,, ..\n"
    ",.,,,,,,,, .  ,,      @@@@@@      ,,,,..\n"
    ", ,,,,,,, . ,,, .   @@@@@@@@@ @    ,,, .\n"
    ", ,,,,,,   ,,, .    @@@@@@@@@@     ,,, .\n"
    " ,,,,,    .   .    @@@@@@@@@@@    .,,,,.\n"
    " ,,,  .    ,  ,,   @@@@@@@@@@@@   .,,,,.\n"
    " ,, .. ,          @@@@@@@@@@@@@@  .,,,, \n"
    " , ...,,          @@@@@@@@@@@@@   . ,,, \n"
    "  ... ,,   @ @   @@@@@@@@@@@@@@   . ,,, \n"
    " ... ,,     @@@  @@@@@@@@@@@@@@  ,. ,,,,\n"
    " ...       @@@@@ @@@@@@@@@@@@@@  ,. ,,,,\n"
    " .. ..    @@@@@@@@@@@@@@@@@@@@@  ,. ,,,,\n"
    "          @@@@@@@@@@@@@@@@@@@@@  ,. ,,,,\n"
    " ,   ,  @@@@@@@@@@@@@@@@@@@@@@   ,..,,,,\n"
    "          @@@@@@@@@@@@@@@@@@@@@  ,. ,,,,\n"
    " .. ..    @@@@@@@@@@@@@@@@@@@@@  ,. ,,,,\n"
    " ...       @@@@@ @@@@@@@@@@@@@@  ,. ,,,,\n"
    " ... ,,     @@@  @@@@@@@@@@@@@@  ,. ,,,,\n"
    "  ... ,,   @ @   @@@@@@@@@@@@@@   . ,,, \n"
    " , ...,,          @@@@@@@@@@@@@   . ,,, \n"
    " ,, .. ,          @@@@@@@@@@@@@@  .,,,, \n"
    " ,,,  .    ,  ,,   @@@@@@@@@@@@   .,,,,.\n"
    " ,,,,,    .   .    @@@@@@@@@@@    .,,,,.\n"
    ", ,,,,,,   ,,, .    @@@@@@@@@@     ,,, .\n"
    ", ,,,,,,, . ,,, .   @@@@@@@@@ @    ,,, .\n"
    ",.,,,,,,,, .  ,,      @@@@@@      ,,,,..\n"
    ",  ,,,,,,,, .. ,,      @@@        ,,, ..\n"
    ",  ,,,,,,,,, ..         @@        ,,, ..\n"
    ",,. ,,,,,,,,, ...       @@   .  .,,, ...\n"
    ",,. ,,,,,,,,, ....  .   @@   ,,  ,,, ...\n"
    ",, . ,,,,,,,,, ....         ,, . ,, ....\n"
    ",, ..,,,,,,,,,, .... ,      , . ,,,.....\n"
    ",,,.. ,,,,,,,,,, ... ,       .. ,, .....\n"
    ",,, .. ,,,,,,,,,, ... ,     .. ,, ......\n";



// ====> Auto Testing Functions <====


// -- Testing Functions and Hardcoded Data;
bool DIFF(Image mine, Image yours, string taskName,
          ostream & out, int & pass_count, int & test_count);


// -- Part A.
void test_validation_and_bounds();
void test_partA_ImageEncodings();

// Tests a pair of encoding and decoding functions.
// Needs a correct image and encoded string pair for the encoding.
// Note image here is not a reference. The test will not mess up
//      the original image or string data.
void testEncoding(bool (*decoder)(istream & in, Image & out),
                  bool (*encoder)(Image & in, ostream & out),
                  Image  correct_image,
                  string correct_file,
                  string test_name,
                  ostream & log,
                  int & pass_count,
                  int & test_count
                  );

// -- Part B.
void test_partB_RenderingAndSampling();

// -- Part C.
void test_partC_CSG();

void concludeTestingSession(string partName,
                            stringstream & log,
                            int pass_count,
                            int test_count
                            );


// -- Useful public functions.

// -- Basic Image functions.
int rowsIn(const Image & i);  // Rows in Image.
int colsIn(const Image & i); // Columns in Image.
void rowsAndColsIn(const Image & i, int & rows, int & cols);

// In: index into i, out row and col values.
// Returns true if the index is in bounds.
bool image_row_col(Image & i, int index, int & row, int & col);
void clearImage(Image & i, int rows, int cols, char val);
// Assumes image is well formed.
void clearImage(Image & i, char val);
void printImageTo(const Image & i, ostream & output, string indent = "");
void prettyPrintImageTo(const Image & i, ostream & output, string indent = "");
void copyImageTo(Image & from, Image & to);

// Converts a '\n' separated String its char array counterpart.
void renderFromString(Image & image, string str);

// Returns the number of characters that differ.
// Returns -1 if the images have different dimensions.
// Marks diff image at every location that differs.
int computeDiff(Image & i1, Image & i2, Image & diff);


// User Read, Evaluate, Process, Loop (REPL)

bool userLoop();
int main()
{
    mkdir("./io"); // Delete this line if working with online gdb.
                   // or if it is causing problems.

    test_validation_and_bounds(); // Task 1.1 and t.2

    while(userLoop())
    {
        cout << "\n\n";
    };
}

bool userLoop()
{
    cout << "---------------------------------------------------------\n";
    cout << " - Enter A, B, or C to test that part of the assignment.\n";
    cout << " - Enter Q if you wish to quit this application.\n";
    cout << "---------------------------------------------------------\n";
    cout << "<Prompt>: ";

    char c;
    cin >> c;
    cout << "-------------\n";
    cout << endl;
    cout << endl;

    // Ensure Capitalization.
    if('a' <= c && c <= 'z'){c = c + 'A' - 'a';};

    // Quit.
    if(c == 'Q')
    {
        return false;
    }

    // -- Part B.
    if(c == 'A')
    {
        test_partA_ImageEncodings();
    }

    // -- Part B.
    if(c == 'B')
    {
        test_partB_RenderingAndSampling();
    }

    // -- Part C.
    if(c == 'C')
    {
        test_partC_CSG();
    }

    return true; // Success, prompt user again.
}


// -- Part A (1.1 and 1.2)
bool assert_image_validity(Image &image, bool shouldBeValid);
bool assert_bounds_check(Image &image, int row, int col, bool theTruth);


// Task group 1 tester (Task 1.1 and 1.2)
// this is always run before task groups 2, 3, and 4.
// Because it is hard ot get correct results anywhere without a
// firm grasp of image validity and how to keep indices in bounds.
void test_validation_and_bounds()
{
    Image good1, good2;
    Image bad1, bad2, bad3, bad4, bad5, bad6;

    renderFromString(good1,  "XXXXX\n");

    renderFromString(good2,  "XXX\n"
                             "XXX\n");

    // Bad: Missing newline char.
    renderFromString(bad1,   "XXXXX");

    renderFromString(bad2,   "XXXXX\n"
                             "XXXXX"); // no '\n'"

    renderFromString(bad3,   "XX\n"
                             "XXX\n"
                    );

    renderFromString(bad4,   "XXXXX\n"
                             "XXXX[\n]"
                    );

    renderFromString(bad5,   ""); // Empty image.

    renderFromString(bad6,   "ABCDEF\n"
                             "GHIJKLM\n"
                             "OPQ\nRST"
                             "UVWXYZ\n"); // various line sizes.

    // Validate the largest images used in this auto-tester.
    Image official1, official2;
    renderFromString(official1, OFFICIAL_TEST_DATA_1);
    renderFromString(official2, OFFICIAL_TEST_DATA_2);

    if(assert_image_validity(good1, true)
       & assert_image_validity(good2, true)
       & assert_image_validity(bad1, false)
       & assert_image_validity(bad2, false)
       & assert_image_validity(bad3, false)
       & assert_image_validity(bad4, false)
       & assert_image_validity(bad5, false)
       & assert_image_validity(bad6, false)
       & assert_image_validity(official1, true)
       & assert_image_validity(official2, true)

       & assert_bounds_check(good2, 0, 0, true)
       & assert_bounds_check(good2, 0, 1, true)
       & assert_bounds_check(good2, 0, 2, true)
       & assert_bounds_check(good2, 1, 0, true)
       & assert_bounds_check(good2, 1, 1, true)
       & assert_bounds_check(good2, 1, 2, true)

       & assert_bounds_check(good2, 0, 1000000, false)
       & assert_bounds_check(good2, 1000000, 1, false)
       & assert_bounds_check(good2, 1, 3, false)
       & assert_bounds_check(good2, -1, 0, false)
       & assert_bounds_check(good2, 1, -1, false)
       & assert_bounds_check(good2, 2, 2, false)
       & assert_bounds_check(good2, 3, 3, false)
       & assert_bounds_check(good2, -21487, -93003, false)

       & assert_bounds_check(official1, 24, 29, true)
       & assert_bounds_check(official1, 25, 29, false)
       & assert_bounds_check(official1, 24, 30, false)
       & assert_bounds_check(official1, 15, 14, true)
       & assert_bounds_check(official1, 8, -10, false)
       & assert_bounds_check(official1, -1, 70, false)
      )
    {
        cout << "Tasks 1.1 and 1.2: PASSED" << endl;
        cout << " - Your validImage and checkBounds functions work." << endl;
        cout << endl;
        cout << "You may test other parts of the assignment as follows:" << endl;
    }
    else
    {
        cout << endl;
        cout << "Recommendation:" << endl;
        cout << endl;
        cout << "  - Fix your validImage and checkBounds functions first," << endl;
        cout << "    before attempting any other task!" << endl;
        cout << endl;
        cout << "    You can call those functions while developing the" << endl;
        cout << "    other tasks to verify that your operations are safe" << endl;
        cout << "    and don't violate the integrity of the images they" << endl;
        cout << "    are processing!" << endl;
        cout << endl;
        cout << "Task 1.1 and/or task 1.2 FAILED some tests. SEE ABOVE!" << endl;
        cout << endl;
        cout << "LOOK ABOVE!, then Quit and go work on task 1.1 and 1.2!" << endl;
        cout << "or you may test the other parts at your own risk like so:" << endl;
    }
}


// Task 1.1 Tester.
bool assert_image_validity(Image &image, bool shouldBeValid)
{
    Image temp;
    copyImageTo(image, temp);
    if(validateImage(temp) != shouldBeValid)
    {
        cout << "Task 1.1:|Validation failed for this ";
        if(!shouldBeValid){cout << "not ";}
        cout << "valid image:";
        cout << endl;

        cout << "         +--------------------------------------";
        if(!shouldBeValid){cout << "----";}
        cout << endl;

        printImageTo(image, cout, "         |");

        cout << "         +--------------------------------------";
        if(!shouldBeValid){cout << "----";}
        cout << endl;

        return false;
    }

    return true;
}


// Task 1.2 Tester.
bool assert_bounds_check(Image &image, int row, int col, bool mine)
{
    Image temp;
    copyImageTo(image, temp);
    bool yours = checkBounds(temp, row, col);

    if(yours != mine)
    {
        cout << "TASK 1.2:|Bounds Check failed." << endl;
        cout << "         |(row: " << row << ", " << "col: " << col << ") ";
        if(mine)
        {
            cout << "exists in image:" << endl;
        }
        else
        {
            cout << "is out of bounds for the image:" << endl;
        }
        cout << "         +----------------------------------" << endl;
        printImageTo(image, cout,
                "         |");
        cout << "         +----------------------------------" << endl;
        return false;
    }

    return true;
}


// Tasks 2.1 - 2.4
void test_partA_ImageEncodings()
{
    // Declare and initialize the test data from the PDF document.
    Image imageA, imageB;
    renderFromString(imageA, "0123456\n"
                             "789ABCD\n"
                             "EFGHIJK\n"
                    );

    renderFromString(imageB, "+++++\n"
                             "+---+\n"
                             "+---+\n"
                             "+++++\n"
                    );

    string fileA1, fileA2, fileA3, fileA4;

    fileA1 = "BIF_01 3 7 0123456789ABCDEFGHIJK";
    fileA2 = "BIF_02 7 3 07E18F29G3AH4BI5CJ6DK";
    fileA3 = "BIF_03 7 0123456789ABCDEFGHIJK\n";
    fileA4 = "BIF_04 3 7 01 11 21 31 41 "
               "51 61 71 81 91 A1 B1 C1 "
               "D1 E1 F1 G1 H1 I1 J1 K1";

    string fileB1, fileB2, fileB3, fileB4;

    fileB1 = "BIF_01 4 5 ++++++---++---++++++";
    fileB2 = "BIF_02 5 4 +++++--++--++--+++++";
    fileB3 = "BIF_03 5 ++++++---++---++++++\n";
    fileB4 = "BIF_04 4 5 +6 -3 +2 -3 +6";

    cout << "Now testing Part A" << endl;
    cout << endl;
    cout << endl;

    // Test using bogo encoder???

    int pass_count = 0;
    int test_count = 0;
    int & pc = pass_count;
    int & tc = test_count;

    stringstream log;

    log << "--- Testing BIF_01 ---" << endl;
    testEncoding(decode_image_bif_01, encode_image_bif_01,
                 imageA, fileA1, "BIF_01_A", log, pc, tc);
    log << endl;
    testEncoding(decode_image_bif_01, encode_image_bif_01,
                 imageB, fileB1, "BIF_01_B", log, pc, tc);
    log << endl;
    log << endl;
    log << "--- Testing BIF_02 ---" << endl;
    log << endl;
    testEncoding(decode_image_bif_02, encode_image_bif_02,
                 imageA, fileA2, "BIF_02_A", log, pc, tc);
    log << endl;
    testEncoding(decode_image_bif_02, encode_image_bif_02,
                 imageB, fileB2, "BIF_02_B", log, pc, tc);
    log << endl;
    log << endl;
    log << "--- Testing BIF_03 ---" << endl;
    log << endl;
    testEncoding(decode_image_bif_03, encode_image_bif_03,
                 imageA, fileA3, "BIF_03_A", log, pc, tc);
    log << endl;
    testEncoding(decode_image_bif_03, encode_image_bif_03,
                 imageB, fileB3, "BIF_03_B", log, pc, tc);
    log << endl;
    log << endl;
    log << "--- Testing BIF_4 ---" << endl;
    log << endl;
    testEncoding(decode_image_bif_04, encode_image_bif_04,
                 imageA, fileA4, "BIF_04_A", log, pc, tc);
    log << endl;
    testEncoding(decode_image_bif_04, encode_image_bif_04,
                 imageB, fileB4, "BIF_04_B", log, pc, tc);

    // Send a copy of the log to standard output.
    // Log results to file if possible, etc.
    // Say goodbye to user, etc.
    concludeTestingSession("A", log, pass_count, test_count);
}


// https://theasciicode.com.ar/ascii-printable-characters/
// dot-full-stop-ascii-code-46.html
string visible(string str)
{
    if(str == "")
    {
        return "[EMPTY STRING]";
    }

    stringstream in(str);
    stringstream out;

    while(!in.eof())
    {
        char c = in.get();
        if(c == ' ')
        {
            out << ' ';

            // Uncomment any of these if you have
            // trouble seeing spaces...
            //out << '_';
            //out << '.';

            //out << (char)176; // tone.

            //out << (char)220; // top block
            //out << (char)219; // Middle block.
            //out << (char)223; // bottom block.

            //out << (char)254; // square.
        }
        else if (c == '\t')
        {
            out << "[\\t]";
        }
        else if (c == '\n')
        {
            out << "[\\n]"; // << endl; // We don't want to actually break the 1D line in this assignment.
        }
        else
        {
            out << c;
        }
    }

    return out.str();
}

void rewindStream(istream & input_stream)
{
    // If this isn't done, it won't rewind.
    input_stream.clear(std::istream::goodbit);

    // Set input iterator to beginning of stream + 0 offset.
    input_stream.seekg (0, input_stream.beg);
}

// Test the given decoding and encoding functions.
void testEncoding(bool (*decode)(istream & in, Image & out),
                  bool (*encode)(Image & in, ostream & out),
                  Image original_image,
                  string original_file,
                  string test_name,
                  ostream & out,
                  int & pass_count,
                  int & test_count
                 )
{
    int & pc = pass_count;
    int & tc = test_count;

    bool fail = false;

    Image temp;
    string file;
    stringstream encoded_string;

    // Test just the encoder.
    copyImageTo(original_image, temp);
    encode(temp, encoded_string);
    file = encoded_string.str();

    string taskLine = string("Task ") + test_name + "(encode): ";
    int len = taskLine.length();

    // A string with enough spaces to get from the left side of a console to the status column.
    // e.g. Task BIF_04_B(encode):
    //     |                      | <-- indent to here.
    string indent = taskLine;
    for(int i = 0; i < len; i++)
    {
        indent[i] = ' ';
    }

    out << taskLine;
    test_count++;
    if(original_file != file)
    {
        out << "ERROR: Incorrect encoded String.\n\n";
        out << indent << "MINE : "  << original_file << "\n\n";
        out << indent << "YOURS: " << file << "\n\n";
        out << indent << "Your String was encoded from this image:\n\n";
        prettyPrintImageTo(original_image, out, indent + string("       "));
        fail = true;
    }
    else
    {
        out << "PASSED." << endl;
        pass_count++;
    }

    // Test just the decoder.
    file = original_file;
    encoded_string.str(file);
    Image image;
    decode(encoded_string, image);
    rewindStream(encoded_string);

    if(!DIFF(original_image, image, test_name + string("(decode)"), out, pc, tc))
    {
        out << indent << "The decoding was from this string:" << endl;
        out << endl;
        out << indent << "  \"" << visible(file) << "\"" << endl;
        out << endl;
        fail = true;
    }

    // Don't attempt the larger tests if the encoding failed.
    // Still count them as fails though.
    if(fail)
    {
        out << "Task " << test_name << "(both 1): " << "skipped due to earlier failure." << endl;
        out << "Task " << test_name << "(both 2): " << "skipped due to earlier failure." << endl;
        test_count = test_count + 2;
        return;
    }

    // -- Test both encoder and decoder.
    Image test_image_1, test_image_2; // Test functions don't get to mess with these...

    renderFromString(test_image_1, OFFICIAL_TEST_DATA_1);
    renderFromString(test_image_2, OFFICIAL_TEST_DATA_2);

    // Use the larger official test images.

    // -- Test encoding an image, then decoding it back to original.
    encoded_string.str("");
    image.clear();
    copyImageTo(test_image_1, temp);

    if(!encode(temp, encoded_string))
    {
        test_count++;
        out << "Task " << test_name << "(both 1): " << "encode failed" << endl;
        out << "                input: \n";
        prettyPrintImageTo(test_image_1, out, "       ");
    }
    else if(!decode(encoded_string, image))
    {
        rewindStream(encoded_string);
        test_count++;
        out << "Task " << test_name << "(both 1): " << "decode failed" << endl;
        out << "                input: " << visible(encoded_string.str()) << endl;
    }
    else // Encode and decode succeeded without errors.
    {
        rewindStream(encoded_string);
        DIFF(test_image_1, image, test_name + string("(both 1)"), out, pc, tc);
    }


    encoded_string.str("");
    image.clear();
    copyImageTo(test_image_2, temp);
    if(!encode(temp, encoded_string))
    {
        test_count++;
        out << "Task " << test_name << "(both 2): " << "encode failed" << endl;
        out << "                input: \n";
        prettyPrintImageTo(test_image_2, out, "       ");
    }
    else if(!decode(encoded_string, image))
    {
        test_count++;
        out << "Task " << test_name << "(both 2): " << "decode failed" << endl;
        out << "                input: " << visible(encoded_string.str()) << endl;
    }
    else // Encode and decode succeeded without errors.
    {
        DIFF(test_image_2, image, test_name + string("(both 2)"), out, pc, tc);
    }
}


// -- Part B.

// As in close enough to be considered equal...
bool close(float val1, float val2)
{
    float diff = val1 - val2;
    if(diff < 0)
    {
        diff = -diff;
    }

    return diff < .0001; // Rounding tolerance...
};

// Char to constant code.
string code(char c)
{
        string my_constant = "";
        if(c == V_IN)
        {
            my_constant = "(V_IN)";
        }
        else if(c == V_PARTIAL)
        {
            my_constant = "(V_PARTIAL)";
        }
        else if(c == V_ODD)
        {
            my_constant = "(V_ODD)";
        }
        else if(c == V_EVEN)
        {
            my_constant = "(V_EVEN)";
        }

        return my_constant;
}

void test_partB_RenderingAndSampling()
{


    //render(image, 200);
    //render(image, 128, 282, sampleFromMandelbrot); // 298
    //render(image, 128, 282, superSampleMandelbrot);

    //render(image, 6, 15, sampleFromMandelbrot);
    //render(image, 25, 69, superSampleMandelbrot);
    //render(image, 25, 69, sampleFromMandelbrot);

    // render(image, 12, 30, sampleFromMandelbrot);


    // -- Define some custom sampling functions for testing.

    char (*sampleFromUnitCircle)(float c_x, float c_y);
    char (*sampleFromUnitSquare)(float c_x, float c_y);

    sampleFromUnitCircle = [](float c_x, float c_y) -> char
    {
        if(c_x*c_x + c_y*c_y <= 1){return V_TRUE;}
        else { return V_FALSE;}
    };

    sampleFromUnitSquare = [](float c_x, float c_y) -> char
    {
        if(c_x*c_x <= 1 && c_y*c_y <= 1){return V_TRUE;}
        else { return V_FALSE;}
    };

    char (*superSampleFromUnitCircle)(float c_x, float c_y, float w, float y);
    char (*superSampleFromUnitTorus)(float c_x, float c_y, float w, float y);
    superSampleFromUnitCircle = [](float c_x, float c_y, float w, float h) -> char
    {
        float x1 = c_x - w/4;
        float x2 = c_x + w/4;
        float y1 = c_y - h/4;
        float y2 = c_y + h/4;

        int samplesInCircle = 0;

        // -- Top row.
        float dist1 = x1*x1 + y1*y1;
        float dist2 = x2*x2 + y1*y1;
        float dist3 = x1*x1 + y2*y2;
        float dist4 = x2*x2 + y2*y2;

        const float min_dist = 0;//.3;
        const float max_dist = 1;

        int sample_1 = min_dist < dist1 && dist1 < max_dist ? 1 : 0;
        int sample_2 = min_dist < dist2 && dist2 < max_dist ? 1 : 0;
        int sample_3 = min_dist < dist3 && dist3 < max_dist ? 1 : 0;
        int sample_4 = min_dist < dist4 && dist4 < max_dist ? 1 : 0;

        samplesInCircle = sample_1 + sample_2 + sample_3 + sample_4;

        switch(samplesInCircle)
        {
            case 4:  return '@';
            case 3:
                if(sample_1 == 0){return 'o';}
                if(sample_2 == 0){return ';';}
                if(sample_3 == 0){return 'o';}
                if(sample_4 == 0){return '?';}
            case 2:
                if(sample_1 + sample_2 == 2){return '^';}
                if(sample_3 + sample_4 == 2){return ',';}
                return ';';
            case 1:  return (sample_1 + sample_2 >= 1) ? '`' : '.';
            default: return ' ';

        }
    };

    superSampleFromUnitTorus = [](float c_x, float c_y, float w, float h) -> char
    {
        float x1 = c_x - w/4;
        float x2 = c_x + w/4;
        float y1 = c_y - h/4;
        float y2 = c_y + h/4;

        int samplesInCircle = 0;

        // -- Top row.
        float dist1 = x1*x1 + y1*y1;
        float dist2 = x2*x2 + y1*y1;
        float dist3 = x1*x1 + y2*y2;
        float dist4 = x2*x2 + y2*y2;

        const float min_dist = .3;
        const float max_dist = 1;

        int sample_1 = min_dist < dist1 && dist1 < max_dist ? 1 : 0;
        int sample_2 = min_dist < dist2 && dist2 < max_dist ? 1 : 0;
        int sample_3 = min_dist < dist3 && dist3 < max_dist ? 1 : 0;
        int sample_4 = min_dist < dist4 && dist4 < max_dist ? 1 : 0;

        samplesInCircle = sample_1 + sample_2 + sample_3 + sample_4;

        switch(samplesInCircle)
        {
            case 4:  return '@';
            case 3:
                if(sample_1 == 0){return 'o';}
                if(sample_2 == 0){return ';';}
                if(sample_3 == 0){return 'o';}
                if(sample_4 == 0){return '?';}
            case 2:
                if(sample_1 + sample_2 == 2){return '^';}
                if(sample_3 + sample_4 == 2){return ',';}
                return ';';
            case 1:  return (sample_1 + sample_2 >= 1) ? '`' : '.';
            default: return ' ';

        }
    };


    // -- Define some official goal / ground truth images.
    Image ptCircle, ptSquare, regionCircle;

    renderFromString(ptCircle,
                            ".......@@@@@@@@@@.......\n"
                            "....@@@@@@@@@@@@@@@@....\n"
                            "..@@@@@@@@@@@@@@@@@@@@..\n"
                            ".@@@@@@@@@@@@@@@@@@@@@@.\n"
                            "@@@@@@@@@@@@@@@@@@@@@@@@\n"
                            "@@@@@@@@@@@@@@@@@@@@@@@@\n"
                            "@@@@@@@@@@@@@@@@@@@@@@@@\n"
                            "@@@@@@@@@@@@@@@@@@@@@@@@\n"
                            ".@@@@@@@@@@@@@@@@@@@@@@.\n"
                            "..@@@@@@@@@@@@@@@@@@@@..\n"
                            "....@@@@@@@@@@@@@@@@....\n"
                            ".......@@@@@@@@@@.......\n"
                     );

    renderFromString(ptSquare,
                            "........................\n"
                            "........................\n"
                            "........................\n"
                            "......@@@@@@@@@@@@......\n"
                            "......@@@@@@@@@@@@......\n"
                            "......@@@@@@@@@@@@......\n"
                            "......@@@@@@@@@@@@......\n"
                            "......@@@@@@@@@@@@......\n"
                            "......@@@@@@@@@@@@......\n"
                            "........................\n"
                            "........................\n"
                            "........................\n"
                    );

    renderFromString(regionCircle,
                            "      ,,o@@@@@@;,,      \n"
                            "   .o@@@@@@@@@@@@@@;.   \n"
                            "  o@@@@@@@@@@@@@@@@@@;  \n"
                            " o@@@@@@@@@@@@@@@@@@@@; \n"
                            ";@@@@@@@@@@@@@@@@@@@@@@;\n"
                            "@@@@@@@@@@@@@@@@@@@@@@@@\n"
                            "@@@@@@@@@@@@@@@@@@@@@@@@\n"
                            ";@@@@@@@@@@@@@@@@@@@@@@;\n"
                            " o@@@@@@@@@@@@@@@@@@@@? \n"
                            "  o@@@@@@@@@@@@@@@@@@?  \n"
                            "   `o@@@@@@@@@@@@@@?`   \n"
                            "      ^^o@@@@@@?^^      \n"
                    );

    Image regionCircleOffset, regionTorus, regionTorusOffset;

    renderFromString(regionCircleOffset,
        "                             \n"
        "           .,,,,,,,,.        \n"
        "        .o@@@@@@@@@@@@;,     \n"
        "      .@@@@@@@@@@@@@@@@@@.   \n"
        "     o@@@@@@@@@@@@@@@@@@@@;  \n"
        "    ;@@@@@@@@@@@@@@@@@@@@@@; \n"
        "    @@@@@@@@@@@@@@@@@@@@@@@@ \n"
        "   .@@@@@@@@@@@@@@@@@@@@@@@@;\n"
        "   `@@@@@@@@@@@@@@@@@@@@@@@@;\n"
        "    o@@@@@@@@@@@@@@@@@@@@@@@ \n"
        "    `@@@@@@@@@@@@@@@@@@@@@@` \n"
        "     `@@@@@@@@@@@@@@@@@@@@`  \n"
        "       ^@@@@@@@@@@@@@@@@^    \n"
        "         ^^@@@@@@@@@@^^      \n"
        "              ^^^^           \n"
    );

    renderFromString(regionTorus,
        "        ,,,@@@@@@@@,,,        \n"
        "     ,o@@@@@@@@@@@@@@@@;,     \n"
        "   ,@@@@@@@@@@@@@@@@@@@@@@,   \n"
        " .o@@@@@@@?^^`  `^^o@@@@@@@;. \n"
        " @@@@@@@?`          `o@@@@@@@ \n"
        "o@@@@@@`              `@@@@@@;\n"
        "@@@@@@@                @@@@@@@\n"
        "@@@@@@@                @@@@@@@\n"
        "o@@@@@@.              .@@@@@@?\n"
        " @@@@@@@;.          .o@@@@@@@ \n"
        " `o@@@@@@@;,,.  .,,o@@@@@@@?` \n"
        "   ^@@@@@@@@@@@@@@@@@@@@@@^   \n"
        "     ^o@@@@@@@@@@@@@@@@?^     \n"
        "        ^^^@@@@@@@@^^^        \n"
    );

    renderFromString(regionTorusOffset,
        "          ,,,,,,         \n"
        "      ,o@@@@@@@@@@;,.    \n"
        "    o@@@@@@@@@@@@@@@@;.  \n"
        "  .@@@@@@?^^  `^^@@@@@@. \n"
        " .@@@@@?          o@@@@@.\n"
        " ;@@@@?            o@@@@@\n"
        " @@@@@;            ;@@@@@\n"
        " ;@@@@@            o@@@@?\n"
        "  o@@@@@,        .o@@@@@`\n"
        "   o@@@@@@;,,,,o@@@@@@?` \n"
        "    `o@@@@@@@@@@@@@@@^   \n"
        "       ^^@@@@@@@@?^`     \n"
        "                         \n"
    );

    // -- Task 3.1 Tester.
    void (*test_escaped)(bool mine, float z_x, float z_y,
                         stringstream & out, int & pc, int & tc);

    // -- Task 3.2 Tester.
    void (*test_iterate)(float my_x,   float my_y,

                         float z_x,    float z_y,
                         float c_x,    float c_y,

                         stringstream & out, int & pc, int & tc);

    // -- Task 3.3 Tester.
    void (*test_escapeTime)(int mine,
                            float c_x, float c_y, int give_up,
                            stringstream & out, int & pc, int & tc);

    // -- Task 3.4 Tester.
    void (*test_sampleBrot)(char mine,
                            float c_x, float c_y,
                            stringstream & out, int & pc, int & tc);

    // -- Task 3.5 Tester.
    void (*test_lerp)(float mine,
                      float val1, float val2, float time,
                      stringstream & out, int & pc, int & tc);

    // -- Task 3.6 Tester.
    void (*test_pointRender)(
                           Image & mine, Image & yours, Bounds bounds,
                           char (*pointSampler)(float, float),
                           string taskname, stringstream & log,
                           int & pc, int & tc);

    // -- Task 3.8 Tester.
    void (*test_superBrot)(char mine,
                           float c_x, float c_y,
                           float region_w, float region_h,
                           stringstream & out, int & pc, int & tc);

    // -- Task 3.9 Tester.
    void (*test_regionRender)(
                           Image & mine, Image & yours, Bounds bounds,
                           char (*regionSampler)(float, float,
                                                 float, float),
                           string taskname, stringstream & log,
                           int & pc, int & tc);


    // Run this after all tests for each task.
    void (*finishTask)(string task_name,
                       stringstream & task_log,
                       int & task_pass, int & task_tests,
                       stringstream & all_log,
                       int & all_pass,  int & all_tests
                      ); // all log.

    finishTask = [](string task_name,
                    stringstream & task_log,
                    int & task_pass, int & task_tests,
                    stringstream & all_log,
                    int & all_pass,  int & all_tests
                    ) // all log.
    {
        all_pass  += task_pass;
        all_tests += task_tests;

        if(task_pass == task_tests && task_name.size() > 0)
        {
            all_log << "Task " << task_name << ": All ";
            all_log << task_tests << " Test";
            if(task_tests != 1)
            {
                all_log << "s"; // plurality grammar.
            }
            all_log << " PASSED." << endl;
        }
        else
        {
            // Only log the task if it has failures.
            // Otherwise we'll avoid verbose lists of PASSED messages.
            all_log << task_log.str();
        }

        // Revert the task counts and logs
        // to prepare for another group of tests for the next task.
        task_log.str("");
        task_pass  = 0;
        task_tests = 0;

    };



    stringstream log;
    stringstream task_log;

    int all_pass_count = 0;
    int all_test_count = 0;
    int task_pass_count = 0; // Temp test buffers
    int task_test_count = 0;

    // Shorthands, since we'll be passing these a lot.
    stringstream & tl = task_log;
    int & apc = all_pass_count;
    int & atc = all_test_count;
    int & pc = task_pass_count;
    int & tc = task_test_count;


    cout << "Now testing Part B" << endl;
    cout << endl;
    cout << endl;

    // Task 3.1
    test_escaped = [](bool mine, float z_x, float z_y,
                      stringstream & out, int & pass_c, int & test_c)
    {
        test_c++;

        if(escaped(z_x, z_y) == mine)
        {
            //out << "PASSED" << endl;
            pass_c++;
            return;
        }

        out << "Task 3.1: ";
        out << "In: (z_x: " << z_x << ", z_y: " << z_y << ")" << endl;
        out << "My Output  : " <<  (mine ? "true" : "false")  << endl;
        out << "Your Output: " << (!mine ? "true" : "false")   << endl;
    };


    test_escaped(true,   2.000001, 0, log, pc, tc);
    test_escaped(false,  1.999999, 0, log, pc, tc);
    test_escaped(true,   1.8, 0.9, log, pc, tc);
    test_escaped(false, -1.8, -.8, log, pc, tc);

    test_escaped(true,   1.6, -1.3, log, pc, tc);
    test_escaped(false,  1.5, -1.3, log, pc, tc);

    test_escaped(true,  -1.6, +1.3, log, pc, tc);
    test_escaped(false, -1.5, +1.3, log, pc, tc);

    test_escaped(true,  1.2, +1.6, log, pc, tc);
    test_escaped(false, 1.2, -0.5, log, pc, tc);

    finishTask("3.1 (escaped)", tl, pc, tc, log, apc,  atc);


    // -- Task 3.2 Tester.
    test_iterate = [](float my_x,   float my_y,

                      float z_x,    float z_y,
                      float c_x,    float c_y,

                      stringstream & out, int & pass_c, int & test_c)
    {
        test_c++;

        float your_x;
        float your_y;

        iterate(z_x, z_y, c_x, c_y, your_x, your_y);

        if(close(my_x, your_x) && close(my_y, your_y))
        {
            //out << "PASSED" << endl;
            pass_c++;
            return;
        }

        out << std::fixed;
        out << std::setprecision(4);
        out << "Task 3.2: ";
        out <<           "In: (z_x: " << z_x << ", z_y: " << z_y << ")" << endl;
        out << "              (c_x: " << c_x << ", c_y: " << c_y << ")" << endl;
        out << endl;
        out << "          Out(MINE):  " << "(my_x_out:   " << my_x   << ", my_y_out:   " << my_y <<  ")" << endl;
        out << "          OUT(YOURS): " << "(your_x_out: " << your_x << ", your_y_out: " << your_y << ")" << endl;
        out << endl;
    };

    // Task 3.2: Iterate.
    test_iterate(0, 0, 0, 0, 0, 0, log, pc, tc);
    test_iterate(1.7475, 1.1366, 1.5, .45, -.3, -.2134, log, pc, tc);
    test_iterate(-1.5321, -1.3616, -.523, 1.34, -.01, .04, log, pc, tc);
    test_iterate(-106.76, 203.2, 8.2, 13, -5, -10, log, pc, tc);
    test_iterate(-.3903, .1301, .2493, -.8902, .34, .574, log, pc, tc);
    finishTask("3.2 (iterate once)", tl, pc, tc, log, apc,  atc);

    // Task 3.3 Tester.
    test_escapeTime = [](int mine,
                         float c_x, float c_y, int give_up,
                         stringstream & out, int & pass_c, int & test_c)
    {
        test_c++;

        int yours = mandelbrotEscapeTime(c_x, c_y, give_up);

        if(yours == mine)
        {
            //out << "PASSED" << endl;
            pass_c++;
            return;
        }

        out << std::fixed;
        out << std::setprecision(4);
        out << "Task 3.3: ";
        out <<           "In: (c_x: " << c_x << ", c_y: " << c_y << ", give_up: " << give_up << ")" << endl;
        out << endl;
        out << "          Escape Time (MINE):  " << mine  << endl;
        out << "                      (YOURS): " << yours << endl;
        out << endl;
    };

    test_escapeTime(2, 1, -.0219, 100, log, pc, tc);
    test_escapeTime(3,  .9456, .0123, 100, log, pc, tc);

    test_escapeTime(5,  .5, .5, 100, log, pc, tc);
    test_escapeTime(6, -0.7645841280, 0.5218677424, 100, log, pc, tc);

    test_escapeTime(17, -1.2, -.3045, 100, log, pc, tc);
    test_escapeTime(14, -1.2, -.3045, 13, log, pc, tc);

    test_escapeTime(5,  0,  0, 4, log, pc, tc);
    test_escapeTime(101,  0,  0, 100, log, pc, tc);
    test_escapeTime(101, -0.64897, 0.36173, 100, log, pc, tc);
    test_escapeTime(187, -0.64897, 0.36173, 10000, log, pc, tc);

    finishTask("3.3 (escape time)", tl, pc, tc, log, apc,  atc);


    // -- Task 3.4 Tester.
    test_sampleBrot = [](char mine,
                         float c_x, float c_y,
                         stringstream & out, int & pass_c, int & test_c)
    {
        test_c++;

        char yours = sampleFromMandelbrot(c_x, c_y);

        if(yours == mine)
        {
            //out << "PASSED" << endl;
            pass_c++;
            return;
        }

        out << std::fixed;
        out << std::setprecision(4);
        out << "Task 3.4: ";
        out <<           "In: (c_x: " << c_x << ", c_y: " << c_y << ")" << endl;
        out << endl;
        out << "          Out: (MINE):  \'" << mine  << "\' " << code(mine)  << endl;
        out << "               (YOURS): \'" << yours << "\' " << code(yours) << endl;
        out << endl;
    };

    test_sampleBrot(V_IN,  0,  0, log, pc, tc);
    test_sampleBrot(V_ODD,  .5, .5, log, pc, tc);
    test_sampleBrot(V_IN, -0.64897, 0.36173, log, pc, tc);

    test_sampleBrot(V_ODD, -1.2, -.3045, log, pc, tc);
    test_sampleBrot(V_EVEN, 1, -.0219, log, pc, tc);
    test_sampleBrot(V_ODD,  .9456, .0123, log, pc, tc);
    test_sampleBrot(V_EVEN, -0.7645841280, 0.5218677424, log, pc, tc);

    finishTask("3.4 (point sample Mandelbrot set)",
               tl, pc, tc, log, apc,  atc);

    // Task 3.5 Tester
    test_lerp = [](float mine,
                   float val1, float val2, float time,
                   stringstream & out, int & pass_c, int & test_c)
    {
        test_c++;

        float yours = lerp(val1, val2, time);

        if(close(yours, mine))
        {
            //out << "PASSED" << endl;
            pass_c++;
            return;
        }

        out << std::fixed;
        out << std::setprecision(4);
        out << "Task 3.4: ";
        out <<           "In:  (val1: " << val1 << ", val2: " << val2 << ", time)" << ")" << endl;
        out << endl;
        out << "          Out: (MINE):  " << mine  << endl;
        out << "               (YOURS): " << yours << endl;
        out << endl;
    };

    test_lerp(0, 0, 0, 10000, log, pc, tc);
    test_lerp(.5, 0, 1, .5, log, pc, tc);
    test_lerp(1.74, 1, 2, .74, log, pc, tc);
    test_lerp(-4.341, -4, -5, .341, log, pc, tc);

    test_lerp(11.2861, -3, 67.34, .2031, log, pc, tc);
    test_lerp(27.1193, 300.345, -.002, .9097, log, pc, tc);

    finishTask("3.5 (linear interpolation)", tl, pc, tc, log, apc, atc);


    Image image;

    // 3.6 (render from point sampler) tester.
    test_pointRender = [](Image & mine, Image & yours, Bounds bounds,
                          char (*pointSampler)(float, float),
                          string taskname, stringstream & log,
                          int & pc, int & tc)
    {
        render(yours, bounds, pointSampler);
        string pre = taskname;
        if(!DIFF(mine, yours, pre, log, pc, tc))
        {
            pre = string("Task ") + pre + string(": ");
            int len = pre.length();
            for(int i = 0; i < len; i++)
            {
                pre[i] = ' ';
            }


            log << setprecision(2);
            log << pre;
            log << "Image rendered from these bounds:" << endl;
            log << endl;
            log << pre;
            log << "  (x1: " << bounds[0] << ", y1: " << bounds[1]
                <<  ", x2: " << bounds[2] << ", y2: " << bounds[3]
                << ")" << endl;
            log << endl;
        }
    };

    // Task 3.6 testing code.
    clearImage(image, 12, 24, '?');
    test_pointRender(ptCircle, image, Bounds({-1, -1, 1, 1}),
                     sampleFromUnitCircle, "3.6 (circle):", tl, pc, tc);

    clearImage(image, 12, 24, '?');
    test_pointRender(ptSquare, image, Bounds({-2, -2, 2, 2}),
                     sampleFromUnitSquare, "3.6 (square):", tl, pc, tc);

    finishTask("3.6 (Mandelbrot point sampler)", tl, pc, tc, log, apc, atc);


    // Task 3.7 Testing Code.
    image.clear();
    Image goal;

    renderFromString(goal, OFFICIAL_BROT_25_69_POINTS);
    render(image, 25, 69, sampleFromMandelbrot);
    DIFF(goal, image, "3.7", tl, pc, tc);

    renderFromString(goal, OFFICIAL_BROT_80_30_POINTS);
    render(image, 80, 30, sampleFromMandelbrot);
    DIFF(goal, image, "3.7", tl, pc, tc);

    finishTask("3.7 (Mandelbrot image from point samples)", tl, pc, tc, log, apc, atc);

    // -- Task 3.8 Tester.
    test_superBrot = [](char mine,
                        float c_x, float c_y,
                        float region_w, float region_h,
                        stringstream & out, int & pass_c, int & test_c)
    {
        test_c++;

        char yours = superSampleMandelbrot(c_x, c_y, region_w, region_h);

        if(yours == mine)
        {
            //out << "PASSED" << endl;
            pass_c++;
            return;
        }

        out << std::fixed;
        out << std::setprecision(2);
        out << "Task 3.8: (superSampleMandelbrot)";
        out << endl;
        out <<           "In: (c_x: " << c_x << ", c_y: " << c_y << ")" << endl;
        out << endl;
        out << "          Out: (MINE):  \'" << mine  << "\' " << code(mine) << endl;
        out << "               (YOURS): \'" << yours << "\' " << code(yours)<< endl;
        out << endl;
    };

    // Task 3.8
    float len = .02;
    test_superBrot(V_ODD,  5,  4, len, len, log, pc, tc); // 1 time to escape.
    test_superBrot(V_ODD,  -12, 0, len, len, log, pc, tc);

    test_superBrot(V_IN,  0,  0, len, len, log, pc, tc);
    test_superBrot(V_ODD,  0,  0, 50, 50, log, pc, tc);
    test_superBrot(V_ODD,  .5, .5, len, len, log, pc, tc);
    test_superBrot(V_PARTIAL, -0.64897, 0.36173, len, len, log, pc, tc);

    test_superBrot(V_PARTIAL, -1.2, -.3045, len, len, log, pc, tc);
    test_superBrot(V_PARTIAL, 1, -.0219, len, len, log, pc, tc);
    test_superBrot(V_ODD,  .9456, .0123, len, len, log, pc, tc);
    test_superBrot(V_EVEN, -0.7645841280, 0.5218677424, len, len, log, pc, tc);

    finishTask("3.8 (Mandelbrot region sampler)", tl, pc, tc, log, apc, atc);

    // Task 3.9 Tester.

    test_regionRender = [](Image & mine, Image & yours, Bounds bounds,
                           char (*regionSampler)(float, float,
                                                 float, float),
                           string taskname, stringstream & log,
                           int & pc, int & tc)
    {
        render(yours, bounds, regionSampler);
        string pre = string("3.9 ") + taskname;
        if(!DIFF(mine, yours, pre, log, pc, tc))
        {
            pre = string("Task ") + pre + string(": ");
            int len = pre.length();
            for(int i = 0; i < len; i++)
            {
                pre[i] = ' ';
            }


            log << setprecision(2);
            log << pre;
            log << "Image Rendered from these bounds:" << endl;
            log << endl;
            log << pre;
            log << "  (x1: " << bounds[0] << ", y1: " << bounds[1]
                <<  ", x2: " << bounds[2] << ", y2: " << bounds[3]
                << ")" << endl;
            log << endl;
        }
    };

    // Task 3.9.
    clearImage(image, 12, 24, '?');
    test_regionRender(regionCircle, image, Bounds{-1, -1, 1, 1},
                      superSampleFromUnitCircle, "(circle region)", tl, pc, tc);

    clearImage(image, 15, 29, '?');
    test_regionRender(regionCircleOffset, image,
                      Bounds({-1.3, -1.2, 1.05, 1.1}),
                      superSampleFromUnitCircle,
                      "(circle bounds)", tl, pc, tc);

    clearImage(image, 14, 30, '?');
    test_regionRender(regionTorus, image, Bounds({-1, -1, 1, 1}),
                      superSampleFromUnitTorus,
                      "(O' region)", tl, pc, tc);

    clearImage(image, 13, 25, '?');
    test_regionRender(regionTorusOffset, image,
                      Bounds({-1.1, -1.1, 1, 1.15}),
                      superSampleFromUnitTorus,
                      "(O' bounds)", tl, pc, tc);

    finishTask("3.9 (render from region samplers)",
               tl, pc, tc, log, apc, atc);

    // Task 3.10 Testing Code.
    image.clear();
    render(image, 25, 50, superSampleMandelbrot);
    renderFromString(goal, OFFICIAL_BROT_25_50_REGIONS);
    DIFF(goal, image, "3   (25 by 50)", tl, pc, tc);
    render(image, 41, 40, superSampleMandelbrot);
    renderFromString(goal, OFFICIAL_BROT_41_40_REGIONS);
    DIFF(goal, image, "3   (41 by 40)", tl, pc, tc);
    finishTask("3   (Mandelbrot image from region samples)", tl, pc, tc, log, apc, atc);


    concludeTestingSession("B", log, all_pass_count, all_test_count);

    if(all_pass_count < all_test_count)
    {
        cout << endl;
        cout << endl;
        cout << "NOTE: It may be hard to fully pass these part B tests." << endl;
        cout << "      at 0% wrong. Just try to get as close as you can." << endl;
        cout << endl;
        cout << "NOTE: 'Wrong' could mean me or you. Let me know" << endl;
        cout << "      if you believe there is a bug in my tests." << endl;
    }
}


// -- Part C.
void test_partC_CSG()
{
    Image a,   b;
    Image ALL, NONE;

    renderFromString(a,     "@@@@@@@@....\n"
                            "@@@@@@@@....\n"
                            "@@@@@@@@....\n"
                            "@@@@@@@@....\n"
                            "@@@@@@@@....\n"
                            "@@@@@@@@....\n"
                    );

    renderFromString(b,     "........@...\n"
                            "........@@..\n"
                            "@@@@@@@@@@@@\n"
                            "@@@@@@@@@@@@\n"
                            "........@@..\n"
                            "........@...\n"
                    );

    renderFromString(ALL,   "@@@@@@@@@@@@\n"
                            "@@@@@@@@@@@@\n"
                            "@@@@@@@@@@@@\n"
                            "@@@@@@@@@@@@\n"
                            "@@@@@@@@@@@@\n"
                            "@@@@@@@@@@@@\n"
                    );

    renderFromString(NONE,  "............\n"
                            "............\n"
                            "............\n"
                            "............\n"
                            "............\n"
                            "............\n"
                    );


    // Secondary Images.
    Image NOTa, NOTb, aANDb, aORb, aXORb;

    renderFromString(NOTa,
                            "........@@@@\n"
                            "........@@@@\n"
                            "........@@@@\n"
                            "........@@@@\n"
                            "........@@@@\n"
                            "........@@@@\n"
                    );

    renderFromString(NOTb,
                            "@@@@@@@@.@@@\n"
                            "@@@@@@@@..@@\n"
                            "............\n"
                            "............\n"
                            "@@@@@@@@..@@\n"
                            "@@@@@@@@.@@@\n"
                    );

    renderFromString(aANDb,
                            "............\n"
                            "............\n"
                            "@@@@@@@@....\n"
                            "@@@@@@@@....\n"
                            "............\n"
                            "............\n"
                    );

    renderFromString(aORb,
                            "@@@@@@@@@...\n"
                            "@@@@@@@@@@..\n"
                            "@@@@@@@@@@@@\n"
                            "@@@@@@@@@@@@\n"
                            "@@@@@@@@@@..\n"
                            "@@@@@@@@@...\n"
                    );

    renderFromString(aXORb,
                            "@@@@@@@@@...\n"
                            "@@@@@@@@@@..\n"
                            "........@@@@\n"
                            "........@@@@\n"
                            "@@@@@@@@@@..\n"
                            "@@@@@@@@@...\n"
                    );


    stringstream log;
    int pass_count = 0;
    int test_count = 0;

    int & pc = pass_count;
    int & tc = test_count;

    log << "Now Testing Part C\n";
    log << "\n\n--(PDF Examples)--\n\n";
    DIFF(NOTa, !a, "4.1 (NOT)    ",     log, pc, tc);
    DIFF(NOTb, !b, "4.1 (NOT)    ",     log, pc, tc);

    DIFF(aANDb, a & b, "4.2 (AND)    ", log, pc, tc);
    DIFF(aORb,  a | b, "4.3 (OR)     ", log, pc, tc);
    DIFF(aXORb, a ^ b, "4.4 (XOR)    ", log, pc, tc);

    DIFF(NONE, a & !a, "4   (a & !a) ", log, pc, tc);
    DIFF(NONE, b & !b, "4   (b & !b) ", log, pc, tc);
    DIFF(ALL,  a | !a, "4   (a | !a) ", log, pc, tc);
    DIFF(ALL,  b | !b, "4   (b | !b) ", log, pc, tc);


    Image small1, small2, small_ab, small_none, small_all;

    renderFromString(small1,
                            "@@\n"
                            "..\n"
                    );

    renderFromString(small2,
                            "@.\n"
                            "@.\n");

    renderFromString(small_ab,
                            "a.\n"
                            ".b\n");

    renderFromString(small_none,
                            "..\n"
                            "..\n");

    renderFromString(small_all,
                            "@@\n"
                            "@@\n");

    Image smallNOT1, smallNOT2, smallAND, smallOR, smallXOR;
    Image smallORab, smallXORab;

    renderFromString(smallNOT1,
                            "..\n"
                            "@@\n");
    renderFromString(smallNOT2,
                            ".@\n"
                            ".@\n");
    renderFromString(smallAND,
                            "@.\n"
                            "..\n");
    renderFromString(smallOR,
                            "@@\n"
                            "@.\n");
    renderFromString(smallXOR,
                            ".@\n"
                            "@.\n");

    renderFromString(smallORab,
                            "@.\n"
                            "@@\n");

    renderFromString(smallXORab,
                            ".@\n"
                            ".@\n");

    log << "\n\n--(Small Images)--\n" << endl;
    DIFF(smallNOT1, !small1,            "4.1 (NOT)    ", log, pc, tc);
    DIFF(smallNOT2, !small2,            "4.1 (NOT)    ", log, pc, tc);

    DIFF(smallAND,   small1 & small2,   "4.2 (a AND b)", log, pc, tc);
    DIFF(smallOR,    small1 | small2,   "4.3 (a OR  b)", log, pc, tc);
    DIFF(smallXOR,   small1 ^ small2,   "4.4 (a XOR b)", log, pc, tc);

    DIFF(smallAND,   small2 & small1,   "4.2 (b AND a)", log, pc, tc);
    DIFF(smallOR,    small2 | small1,   "4.3 (b OR  a)", log, pc, tc);
    DIFF(smallXOR,   small2 ^ small1,   "4.4 (b XOR a)", log, pc, tc);

    DIFF(smallAND,   small1 & small_ab, "4 (true vals)", log, pc, tc);
    DIFF(smallORab,  small2 | small_ab, "4 (true vals)", log, pc, tc);
    DIFF(smallXORab, small_ab ^ small1, "4 (true vals)", log, pc, tc);


    bool firstRec = true;
    if((a | small1).size() != 0 || (small2 | b).size() != 0)
    {
        if(firstRec){log << '\n'; firstRec = false;}
        log << "\n";
        log << "Recommendation: If your OR(|) function sees two images with different\n"
               "                row or col amounts, then return an image of size 0.\n";
    }

    if((a & small1).size() != 0 || (small2 & b).size() != 0)
    {
        if(firstRec){log << '\n'; firstRec = false;}
        log << "\n";
        log << "Recommendation: If your AND(&) function sees two images with different\n"
               "                row or col amounts, then return an image of size 0.\n";
    }

    if((a ^ small1).size() != 0 || (small2 ^ b).size() != 0)
    {
        if(firstRec){log << '\n'; firstRec = false;}
        log << "\n";
        log << "Recommendation: If your XOR(|) function sees two images with different\n"
               "                row or col amounts, then return an image of size 0.\n";
    }

    if(!firstRec)
    {
        log << "\n";
        log << "(This advice is to avoid undefined behavior,  as your functions may try\n"
               " something dangerous like accessing the smaller image at an array\n"
               " location that is out of bounds!)\n";
    }

    /*
    printImageTo(!small1, log);
    log << endl;
    printImageTo(!small2, log);
    log << endl;
    printImageTo(small1 & small2, log);
    log << endl;
    printImageTo(small1 | small2, log);
    log << endl;
    printImageTo(small1 ^ small2, log);
    log << endl;
    */

    concludeTestingSession("C", log, pass_count,test_count);
}


void concludeTestingSession(string partName,
                            stringstream & log,
                            int pass_count,
                            int test_count
                            )
{
    // Send a copy of the log's contents to cout.
    cout << log.str();

    float pass_percentage = pass_count * 100.0 / test_count;
    cout << std::fixed;
    cout << std::setprecision(2);

    cout << "\n";
    cout << "\n";
    cout << "Part " << partName << "'s testing is complete. (Look above for details)." << endl;
    cout << endl;
    cout << "Your code passed " << pass_count << " of the "
         << test_count << " tests. (" << pass_percentage << " %)\n\n";
    string log_filename = "./io/part_" + partName + "_log.txt";
    ofstream out(log_filename);

    // Try again without the folder?
    if(!out.is_open())
    {
        log_filename = "./part_" + partName + "_log.txt";
        out = ofstream(log_filename);
    }

    if(out.is_open())
    {
        // Stream the log's contents to out.
        // log must be rewinded if this is to be used again.
        out  << log.rdbuf();
        cout << "Log sent to file: " << log_filename << endl;
    }
    out.close();
}


// Assumes image is valid.
// NOTE: rows are calculated by computing both rows and
//       columns then throwing away the column count!
int rowsIn(const Image & image)  // Rows in Image.
{
    int rows;
    int cols;
    rowsAndColsIn(image, rows, cols);
    return rows;
}


// A functions for retrieving both the row count and column
// count at the same time. This avoid the extra work of counting
// the columns twice. Integer division has a similar optimization,
// since in practice the remainder gets computed at the same times
// as the quotient. Its hard to compute one without the other.
void rowsAndColsIn(const Image & image, int & rows, int & cols)
{
    int len = image.size();
    cols = colsIn(image);
    rows = len / (cols + 1);
}


// Columns in Image.
// image.size == (colsIn(image) + 1)*rowsIn(image)
// Extra non-data column is for '\n' characters.
// Undefined return value if image is not valid.
int colsIn(const Image & i)
{
    int col_count = 0;

    for(char c : i)
    {
        if(c == '\n')
        {
            break;
        }
        col_count++;
    }

    return col_count;
}


// Resizes the image to the given dimensions.
// Overwrites all values with the given val
// Writes '\n' characters to the end of every row.
// total size will be rows*(cols + 1).
void clearImage(Image & image, int rows, int cols, char val)
{
    image.clear();

    // Desired Length of image's char vector.
    int len = rows*(cols + 1);

    for(int i = 0; i < len; i++)
    {
        image.push_back(val);
    }

    for(int i = cols; i < len; i += (cols + 1))
    {
        image[i] = '\n';
    }
}


// Overwrites every location in the image with the given char val.
void clearImage(Image & image, char val)
{
    int len = image.size();
    for(int i = 0; i < len; i++)
    {
        if(image[i] != '\n')
        {
            image[i] = val;
        }
    }
}


// "Ugly  / RAW image printer with a few bells and whistles."
// Simply print all of the character data from the image.
void printImageTo(const Image & i, ostream & output, string indent)
{
    if(i.size() == 0)
    {
        output << indent;
        output << "[EMPTY IMAGE \"\"] (0 rows, 0 columns)" << endl;
        return;
    }

    bool first = true;
    for(char c : i)
    {
        // Indent the first char of each actual line.
        if(first){
            first = false;
            output << indent;
        }

        // Add A visible [\n] indicator at the end of each line.
        if(c == '\n')
        {
            first = true;
            output << "[\\n]";
            // Note: The actual '\n' char will be printed below.
        }

        output << c;
    }

    output.flush();
}


// 0 --> 1, 7 --> 1,
// 14 --> 2, 108 -> 3, etc.
int digitsIn(int number)
{
    int digits = 0;
    do
    {
        number = number / 10;
        digits++;
    }while(number > 0);

    return digits;
}


// Print a nice numbered, etc. image.
void prettyPrintImageTo(const Image & image,
                        ostream & out,
                        string pre) // The indent or other prefix to each line of output.
{

    if(image.size() == 0)
    {
        out << pre;
        out << "[EMPTY IMAGE \"\"] (0 rows, 0 columns)" << endl;
        return;
    }

    int rows, cols;
    rowsAndColsIn(image, rows, cols);


    stringstream line0; //  1
    stringstream line1; // 95
    stringstream line2; //496
    stringstream lineTop; // Top ______

    // Add indent due to row label columns.
    for(int i = 0; i <= digitsIn(rows); i++)
    {
        line0 << " ";
        line1 << " ";
        line2 << " ";
        lineTop << " ";
    }

    for(int i = 0; i < cols; i++)
    {
        char d0 = '0' + (i / 100);
        char d1 = '0' + (i % 100 / 10);
        char d2 = '0' + (i % 10);

        d0 = (d0 == '0') ? ' ' : d0;
        d1 = (d1 == '0' && d0 == ' ') ? ' ' : d1;
        //d2 = (d2 == '0' & d1 == ' ') ? ' ' : d2;

        line0 << d0;
        line1 << d1;
        line2 << d2;
        lineTop << "_";
    }

    // hundreds digit for column labels.
    if(cols - 1 >= 100)
    {
        out << pre << line0.rdbuf() << endl;
    }

    // Tens digit for column labels.
    if(cols - 1 >= 10)
    {
        out << pre << line1.rdbuf() << endl;
    }
    // one's digit.
    out << pre << line2.rdbuf() << endl;

    out << pre << lineTop.rdbuf() << endl;

    // Now draw row index | [row of characters] |
    // for each row in image.

    int index = 0;
    for(int row = 0; row < rows; row++)
    {
        out << pre;

        // Right justify row labels.
        int max_digits = digitsIn(rows);
        int current_digits = digitsIn(row);
        while(current_digits < max_digits)
        {
            out << " ";
            current_digits++; // Count the space as a padded 0 digit.
        }

        out << row << '|';

        // Output the row's data.
        for(int col = 0; col < cols; col++)
        {
            out << image[index];
            index++;
        }

        out << "|\n";
        index++; // +1 to skip over the new line char in image.
    }

    out << pre;
    for(int i = 0; i < digitsIn(rows) + 1 + cols + 1; i++)
    {
        out << ((i < digitsIn(rows)) ? ' ' : '^');
    }
    out << endl;

    out.flush();
}


void copyImageTo(Image & from, Image & to)
{
    to.clear();
    for(char c : from)
    {
        to.push_back(c);
    }
}


// Converts a '\n' separated String its char array counterpart.
void renderFromString(Image & image, string str)
{
    image.clear();

    int len = str.length();
    for(int i = 0; i < len; i++)
    {
        image.push_back(str[i]);
    }
}


// Returns true iff images do not differ.
// Returns false if anything goes wrong.
bool DIFF(Image mine, Image yours, string taskName,
          ostream & out, int & pass_count, int & test_count)
{
    test_count++;

    taskName = string("Task ") + taskName + string(": ");
    out << taskName; // no new line here.


    // solve the indenting problems.
    string taskIndent = taskName;
    int len = taskIndent.length();
    for(int i = 0; i < len; i++)
    {
        taskIndent[i] = ' ';
    }
    string newLine = string("\n") + taskIndent;
    string prettyPrintIndent = taskIndent + string("    ");


    Image diff;

    int count = computeDiff(mine, yours, diff);

    if(count == ERROR_EMPTY_IMAGE)
    {
        out << "ERROR: Your Image has not been initialized (size 0)." << newLine;
        out << "       Have you implemented this yet?" << endl;
        out << endl;
        return false;
    }

    if(count == ERROR_DIMENSION_MISMATCH)
    {
        out << "ERROR: Your Image has the wrong dimensions." << endl;
        out << newLine;
        int rows, cols;
        rowsAndColsIn(mine, rows, cols);
        out << "MINE:  " << rows << " rows, " << cols << " columns." << endl;
        out << endl;
        prettyPrintImageTo(mine,  out, prettyPrintIndent);
        out << newLine;

        rowsAndColsIn(yours, rows, cols);
        out << "YOURS: " << rows << " rows, " << cols << " columns." << endl;
        out << endl;
        prettyPrintImageTo(yours,  out, prettyPrintIndent);
        out << "\n";

        return false;
    }

    if(count == 0)
    {
        out << "PASSED.\n";
        pass_count++;
        return true;
    }


    // The Images differ. print DIFF output. (3 images).

    int size = rowsIn(mine)*colsIn(mine);

    out << "Your image differs from mine. (Try to minimize this)" << newLine;
    out << newLine;

    out << "(" << count << " of " << size << " locations differ) ";

    // Output percentage to 2 decimal places.
    out << std::fixed;
    out << std::setprecision(2);
    out << "(" << (count * 100.0 / size) << " % Wrong)" << newLine;

    out << newLine;
    out << "MINE:" << endl;
    out << endl;
    prettyPrintImageTo(mine,  out, prettyPrintIndent);
    out << newLine;
    out << "YOURS:" << endl;
    out << endl;
    prettyPrintImageTo(yours, out, prettyPrintIndent);
    out << newLine;
    out << "X marks the " << count;
    out << " location";
           if(count != 1){out << "s";}
    out << " that" << newLine << "differ";
           if(count == 1){out << "s";}
    out << " between our images:" << newLine;
    out << endl;
    prettyPrintImageTo(diff,  out, prettyPrintIndent);
    out << endl;

    return false;

}


int computeDiff(Image & i1, Image & i2, Image & diff)
{
    diff.clear();

    if(i1.size() == 0 || i2.size() == 0)
    {
        return ERROR_EMPTY_IMAGE;
    }

    if(colsIn(i1) != colsIn(i2) || i1.size() != i2.size())
    {
        return ERROR_DIMENSION_MISMATCH;
    }

    int diff_count = 0;
    int len = i1.size();

    // Note: We don't need to worry about extra '\n' characters, since they should be the same.

    for(int index = 0; index < len; index++)
    {
        if(i1[index] == '\n')
        {
            diff.push_back('\n');
        }
        else if(i1[index] == i2[index])
        {
            diff.push_back('`');
        }
        else
        {
            diff.push_back('X');
            diff_count++;
        }
    }

    return diff_count;
}
//======================================




//--------------------------------------------------------

//          Testing Code Above This Line (MINE)

//--------------------------------------------------------

//               Your code is below (YOURS)

//--------------------------------------------------------

//
// WARNING: Any of your code written above this line will
//          be ignored during grading!

// #OFFICIAL_LINE!

//--------------------------------------------------------




//============================================//
//                    |                       //

//                    |                       //

//  -  -  - Part A: Image Encodings - - - - - //

//                    |                       //

//                    |                       //
//============================================//


// Task 1.1 (Do this first!)
bool validateImage(Image & image)
{
    return false;
}

// -- Task 1.2
bool checkBounds(Image & image, int row, int col)
{
    return false;
}


// ====> Encode / Decode Functions. <====

// Task 2.1
bool decode_image_bif_01(istream & in, Image & out)
{
    return false;
}


// Task 2.1
bool encode_image_bif_01(Image & image, ostream & out)
{
    return false;
}



// Task 2.2
bool decode_image_bif_02(istream & in, Image & out)
{
    return false;
}


// Task 2.2
bool encode_image_bif_02(Image & in, ostream & out)
{
    return false;
}



// Task 2.3
bool decode_image_bif_03(istream & in, Image & out)
{
    return false;
}


// Task 2.3
bool encode_image_bif_03(Image & in, ostream & out)
{
    return false;
}



// Task 2.4
bool decode_image_bif_04(istream & in, Image & out)
{
    return false;
}


// Task 2.4
bool encode_image_bif_04(Image & in, ostream & out)
{
    return false;
}




//==========================================//
//                  |                       //

//                  |                       //

// - Part B: Image Sampling and Rendering - //

//                  |                       //

//                  |                       //
//==========================================//


// Task 3.1
bool escaped(float z_x, float z_y)
{
    return true;
}


// Task 3.2
void iterate(float z_x, float z_y,
             float c_x, float c_y,
             float & x_out, float & y_out)
{
    x_out = 0xBAD*1.0; // FIXME!
    y_out = 0xBAD*1.0;
}


// Task 3.3
// Counts how many iterations it
// takes before the given point escapes.
int mandelbrotEscapeTime(float c_x, float c_y, int give_up)
{
    return -1; // A correct answer will never be negative...
               // It will be between 1 and (give_up + 1)
}


// Task 3.4
char sampleFromMandelbrot(float c_x, float c_y)
{
    return V_IN;

    // Use task 3.3!
    // return V_EVEN and V_ODD for points that escape...
    // Not every point is 'IN' the set.
}


// Task 3.5
float lerp(float val1, float val2, float time)
{
    return -1.0*0xBAD; // Use formula...
}


// Task 3.6
bool render(Image & image, Bounds bounds,
            char (*sampleAt)(float, float))
{
    // Maybe check whether validateImage(image) is true...
    return false; // invalid image or bounds.size() != 4...

    //for() rows
    //for() columns
    //
    //  Maybe check to make sure checkBounds(image, row, col) is true
    //  Before trying to access the image...
    //  Or at least check that 0 <= index < image.size();
    //  This is a good 'Sanity check'
    //
    //  (c_x, c_y) = ...
    //  image[index] = sampleAt(c_x, c_y)
    //
    // return true.
}


// Task 3.7
bool render(Image & image, int rows, int cols,
            char (*pointSampler)(float, float))
{
    // Check for invalid rows and columns...
    return false;

    // Short function... call Task 3.6, return its return value.
}


// Task 3.8
char superSampleMandelbrot(float c_x, float c_y,
                           float region_w, float region_h)
{
    return V_IN;

    // +-x----x-+
    // x O    O x
    // |        |
    // |        |
    // x O    O x
    // +-x----x-+

    // Sample the points half way from the center to the corners
    // so the samples are uniformly distributed across the region.

    // Use task 3.3!

    // return V_IN;
    // return V_EVEN;
    // return V_ODD;
    // return V_PARTIAL;
}


//----------
// Task 3.9
//----------
/* This Super samples rectangular regions centered at cx, cy
 * with the give width and height.
 * The top left corner is at point     (cx - width/2, cy - height/2)
 * The bottom right corner is at point (cx + width/2, cy + height/2)
 */
bool render(Image & image, Bounds bounds,
            char (*sampleFromRegion)(float, float, float, float))
{
    return false; // Invalid bounds (not size 4?)

    // Use c_x and c_y formulas.
    // Use region_w and region_h formulas.
    // For all locations:
    // image[index] = sampleFromRegion(c_x, c_y, region_w, region_h;
}


// Task 3.10
bool render(Image & image, int rows, int cols,
            char (* sampleRegion)(float c_x,   float c_y,
                                  float width, float height))
{
    // Look at task 3.7 for inspiration!
    return false;
}




//==========================================//
//                    |                     //

//                    |                     //

// - - - - Part C: Binary Image CSG - - - - //

//                    |                     //

//                    |                     //
//==========================================/


// - Task 4.1 (NOT)
Image operator ! (const Image right)
{
    Image out;
    return out;
}


// Task 4.2 (AND)
Image operator & (const Image left, const Image right)
{
    Image out;
    return out;
}


// TASK 4.3 (OR)
Image operator | (const Image left, const Image right)
{
    Image out;
    return out;
}


// TASK 4.4 (XOR)
Image operator ^ (const Image left, const Image right)
{
    Image out;
    return out;
}
