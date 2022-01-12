#include <iostream>
#include <fstream>
#include <vector>

struct Vector
{
    float x;
    float y;
};
enum class Func
{
    Moveto,
    Vertical,
    Horizontal,
};
struct Function
{
    bool IsRelative();
};
struct Path
{
    std::vector<Point> points;
    std::vector<WidthPoint> widths;
};

int main()
{
    unsigned int width = 120;
    unsigned int height = 140;

    std::ofstream file("test.svg");
    file << R"txt(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: VectorMaker 0 . SVG Version: 6.00 Build 0)  -->
<svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 viewBox="0 0 )txt"
        << width << " " << height << R"txt(" style="enable-background:new 0 0 )txt"
        << width << " " << height << R"txt(;" xml:space="preserve">
<style type="text/css">
	.st0{fill:#808080;}
</style>
<g>
)txt";
    file << "<path d=\"M0,0C0,0,0,0,0,0V15h30V30z\"/>"; // Insert art here
    file << "</g>\n</svg>";
    file.close();
    return 0;
}
