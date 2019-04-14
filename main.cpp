#include <iostream>
#include <cstdio>
#include<cmath>

using namespace std;
bool field[800][300];
int scale;
int last_y =2;
int x_limit = 800;
int y_limit = 300;
float number_points = 0;
int number_figures = 0;

struct figures_info
{
    int number = 0;
    int lines = 0;
    int triangles = 0;
    int squares = 0;
    int circles = 0;
    int not_defined = 0;
}info;

struct figure_points //all points of figure
{
    int rel_x;  //coordinates in relation to first_point
    int rel_y;
    bool boundary = false;
    figure_points *next = NULL; //next point
};

enum figure_type{not_defined,line,triangle,square,circle};

struct figure
{
    figure_type type;
    int square_side; //length of base-square side of figure
    float points_ratio; //ratio points to circumcircle
    float center_dist_ratio;//ratio maximum elonged point to minimum elonged point from mass-center
    int x_origin; //x-coordinate of the most left and upper point of base-square of figure
    int y_origin;//y-coordinate
    int x_first_pt;//x-coordinate of first found point of figure
    int y_first_pt;//y-coordinate
    figure_points *first_point;
    figure *next=NULL;
}first_figure;

figure *current_figure = &first_figure;
figure_points *current_point;


void make_line(int x_origin,int y_origin)
{
    for(int y=y_origin;y<=(y_origin+scale);y++)    //check coordinates inside base square only
    {
        for(int x=x_origin;x<=(x_origin+scale);x++)
        {
            if((x>(x_origin+0.42*scale))&&(x<(x_origin+0.58*scale)))
            {
                if (((y)>=0)&&(y<y_limit)&&(x>=0)&&(x<x_limit))
                field[x][y]=true;
                else cout<<"Out_of_massive!"<<endl;
            }
        }
    }
}

void make_triangle(int x_origin,int y_origin)
{
    int x;
    int y;
    for(y=0;y<=scale;y++)
    {
        for(x=(-scale/2);x<=(scale/2);x++)
        {
            if(y<=(scale-abs(2*x)))
            {
                if (((y_origin-y+scale)>=0)&&(y_origin-y+scale<y_limit)&&((x+x_origin+scale/2)>=0)&&(x+x_origin+scale/2<x_limit))
                field[x+x_origin+scale/2][y_origin-y+scale]=true;
                else cout<<"out_of_massive!"<<endl;
            }
        }
    }
}

void make_square(int x_origin,int y_origin)
{
    for(int x=x_origin;x<=(x_origin+scale);x++)
    {
        for(int y=y_origin;y<=(y_origin+scale);y++)
        {
            if (((y)>=0)&&(y<y_limit)&&(x>=0)&&(x<x_limit))
            field[x][y] = true;
            else cout<<"out_of_massive!"<<endl;
        }
    }
}

void make_circle(int x_origin,int y_origin)
{
    int r = scale/2;
    for(int x=-r;x<(r+1);x++)    //checking as if it's near zero point
    {
        for(int y=-r;y<(r+1);y++)
        {
            if(((x*x)+(y*y))<=(r*r))
            {
                if (((y+y_origin+r)>=0)&&(y+y_origin+r<y_limit)&&(x+x_origin+r>=0)&&(x+x_origin+r<x_limit))
                field[x+x_origin+r][y+y_origin+r]=true;
                else cout<<"out_of_massive!"<<endl;
            }
        }
    }
}

void detect_figure(int x,int y)
{
    field[x][y] = false;
    if(current_point==NULL) cout<<"NULL pointer!"<<endl;
    current_point->rel_x = x - current_figure->x_first_pt;  //set relative coordinates
    current_point->rel_y = y - current_figure->y_first_pt;  //
    //check if surrounding fields are not empty
    for(int i=(y-1);i<(y+2);i++)
        for(int h=(x-1);h<(x+2);h++)
        {
            if((field[h][i] == true)&&(i>=0)&&(i<y_limit)&&(h>=0)&&(h<x_limit)) //also check that inside massive "field" limits
            {
                current_point->next = new(figure_points);
                current_point = current_point->next;
                current_point->next = NULL;
                detect_figure(h,i);
            }
        }
    current_point->next = NULL;
    // as a result, we have information about all points of current figure in list of figure_points type
    //with pointer "first_point" as a start point
}

//it also determines boundary-points
void renew_figure(figure figure_renew)
{
    int x;
    int y;
    bool out_cycle = false;
    x = figure_renew.x_first_pt;
    y = figure_renew.y_first_pt;
    field[x][y] = true;
    current_point = figure_renew.first_point;
    //this cycle fills all field fields with values
    while(current_point->next != NULL)
    {
        current_point = current_point->next;
        if (((x+current_point->rel_x)<x_limit)&&(y+current_point->rel_y<y_limit))
        field[x+current_point->rel_x][y+current_point->rel_y]=true;
        else cout<<"out of massive!"<<endl;
    }
    //this cycle founds boundary-points and shows them
    for(int i=0;i<y_limit;i++)
    {
        for(int h=0;h<x_limit;h++)
            if(field[h][i]==true)
            {
                for(int u=(i-1);u<(i+2);u++)
                {
                    for(int k=(h-1);k<(h+2);k++)
                    {
                        if((field[k][u]==false)||(u<0)||(u>y_limit)||(k<0)||(k>x_limit))
                        {
                            current_point = figure_renew.first_point;
                            while(current_point != NULL)
                            {
                                if(((current_point->rel_x+figure_renew.x_first_pt) == h)&&(current_point->rel_y+figure_renew.y_first_pt) == i)
                                {
                                    current_point->boundary = true;
                                    out_cycle = true;
                                    break;
                                }
                                current_point = current_point->next;
                            }
                        }
                    }
                    if(out_cycle == true)
                        break;
                }
                /*if(out_cycle == true)
                    cout<<"B";
                else cout<<"W";*/
                out_cycle = false;
            }
           // else cout<<".";
        //cout<<endl;
    }
}

//main alghoritms to find out the nature of the figure
void investigate_figure(figure *current_figure)
{
        //to find the side of base-square, find center-point and ratio points/square
        figure_points *current_point = current_figure->first_point;
        int min_x = 0;
        int max_x = 0;
        int min_y = 0;
        int max_y = 0;
        int sum_x = 0;
        int sum_y = 0;
        float min_center_dist = 0;
        float max_center_dist = 0;
        float center_dist = 0;
        float dx; //support variables
        float dy;
        struct center_point
        {
            float x = 0; //coordinates of center-pt in relative coordinates (from zero-point)
            float y = 0;
        }center;

        number_points = 0;
        do
        {
           if(min_x<current_point->rel_x)
                min_x = current_point->rel_x;
           if(max_x>current_point->rel_x)
                max_x = current_point->rel_x;
           if(min_y<current_point->rel_y)
                min_y = current_point->rel_y;
           if(max_y>current_point->rel_y)
                max_y = current_point->rel_y;
           sum_x = sum_x + current_point->rel_x;
           sum_y = sum_y + current_point->rel_y;
           number_points++;
           current_point = current_point->next;
        }while(current_point != NULL);
        //now we can calculate center-point
        center.x = float(sum_x/number_points);
        center.y = float(sum_y/number_points);
        //and square side
        if(abs(max_x-min_x)>abs(max_y-min_y))
            current_figure->square_side = max_x-min_x;
        else current_figure->square_side = max_y-min_y;
        current_figure->square_side = abs(current_figure->square_side)+1;
        //calculating ratio
        current_figure->points_ratio = number_points/pow(current_figure->square_side,2);
        //finding max & min distances from center to boundary
        current_point = current_figure->first_point;
        dx = current_point->rel_x - center.x;   //first point is a boundary anyway, so to have a reference
        dy = current_point->rel_y - center.y;   //we will do first check
        center_dist = sqrt(pow(dx,2)+pow(dy,2));
        min_center_dist = center_dist;
        max_center_dist = center_dist;
        do
        {
            if(current_point->boundary == true)
            {
                dx = current_point->rel_x - center.x;
                dy = current_point->rel_y - center.y;
                center_dist = sqrt(pow(dx,2)+pow(dy,2));
                if(center_dist<min_center_dist)
                    min_center_dist = center_dist;
                if(center_dist>max_center_dist)
                    max_center_dist = center_dist;
            }
            current_point = current_point->next;
        }while(current_point != NULL);
        current_figure->center_dist_ratio = max_center_dist/min_center_dist;
        //calculate ratio of circumcircle area to figure area (using max_center_dist as radius)
        current_figure->points_ratio = number_points/(M_PI*pow(max_center_dist,2));
        //now we have all necessary info about figure to decide what it is
        //cout<<"center-pt: "<<center.x<<" "<<center.y<<endl;
        //cout<<"square-side: "<<current_figure->square_side<<endl;
        cout<<"ratio: "<<current_figure->points_ratio<<endl;
        //cout<<"max boundary dist: "<<max_center_dist<<endl;
        //cout<<"min boundary dist: "<<min_center_dist<<endl;
        cout<<"center dist ratio: "<<current_figure->center_dist_ratio<<endl;
        if((abs(current_figure->points_ratio-1.0)<0.15)&&(abs(current_figure->center_dist_ratio-1.0)<0.15))
        {
            current_figure->type = circle;
            info.circles++;
        }
        else if((abs(current_figure->points_ratio-0.205)<0.15)&&(current_figure->center_dist_ratio>6.0))
        {
            current_figure->type = line;
            info.lines++;
        }
        else if((abs(current_figure->points_ratio-0.643)<0.15)&&(abs(current_figure->center_dist_ratio-1.414)<0.3))
        {
            current_figure->type = square;
            info.squares++;
        }
        else if((abs(current_figure->points_ratio-0.361)<0.15)&&(abs(current_figure->center_dist_ratio-2.271)<1.0))
        {
            current_figure->type = triangle;
            info.triangles++;
        }
        else
        {
            current_figure->type = not_defined;
            info.not_defined++;
        }

}

void reset_field()
{
    for(int i=0;i<y_limit;i++)
    {
        for(int h=0;h<x_limit;h++)
            field[h][i]=false;
    }
}

bool symb;
int mode =0;

int main()
{
    cout<<"Choose action"<<endl<<"1 -- create new massive;"<<endl<<"2 -- figure out species"<<endl;
    cin>>mode;
    FILE *fp;
    switch (mode)
    {
        case 1:
           if((fp = fopen("out_figures","wb"))==NULL)
            {
                printf("Inable to open file.\n");
            }
            for(int i=10;i<12;i++)
            {
                scale = 10*i;
                make_line(5,last_y);
                make_triangle(scale+10,last_y);
                make_circle(scale*2+15,last_y);
                make_square(scale*3+20,last_y);
                last_y = last_y + scale + 5;
            }
            //scale = 195;
            // make_square(2,2);
            // make_line(2,2);
            // make_circle(2,2);
            //make_triangle(2,2);
            for(int y=0;y<y_limit;y++)
            {
                for(int x=0;x<x_limit;x++)
                    if(field[x][y]) putc('1',fp);
                    else putc('0',fp);
            }
            if((fclose(fp))==0)
                cout<<"All OK!"<<endl;
            else cout<<"Error in closing file!"<<endl;

            if((fp=fopen("out_figures","rb"))==NULL)
            {
                printf("Inable to open file to read.\n");
            }
            for(int y=0;y<y_limit;y++)
            {
                for(int x=0;x<x_limit;x++)
                {
                    if(getc(fp)=='1') cout<<"W";
                    else cout<<".";
                }
                cout<<endl;
            }
            if((fclose(fp))==0)
                cout<<"All OK!"<<endl;
            else cout<<"Error in closing file!"<<endl;
           break;
        case 2:
            first_figure.first_point = new(figure_points);
            current_point = first_figure.first_point;
            first_figure.first_point->next = NULL;
            if((fp = fopen("out_figures","rb"))==NULL)
                printf("Inable to open file.\n");
            for(int y=0;y<y_limit;y++)      //fulfill massive with values
            {
                for(int x=0;x<x_limit;x++)
                    if(getc(fp)=='1')
                        field[x][y] = true;
            }
            //next step is going through the massive and... recursion!!!
            for(int y=0;y<y_limit;y++)
            {
                for(int x=0;x<x_limit;x++)
                {
                    if(field[x][y]==true)
                    {
                        current_figure->x_first_pt = x;
                        current_figure->y_first_pt = y;
                        detect_figure(x,y);
                        current_figure->next = new(figure); //next figure is creating at once previous one detected
                        current_figure = current_figure->next;
                        current_figure->next = NULL;
                        current_figure->first_point = new(figure_points);
                        current_figure->first_point->next = NULL; //it means that last figure is the figure with only one point (first point)
                        //this figure will be deleted
                        current_point = current_figure->first_point;
                    }
                }
            }
            //now we finished with detecting of figures. Let's check what has came out
            current_figure = &first_figure;
            while((current_figure->first_point->next != NULL)||(current_figure->next != NULL))
            {
                info.number++;
                current_point = current_figure->first_point;
                number_points = 1;
                while(current_point->next != NULL)
                {
                    number_points++;
                    current_point = current_point->next;
                    if(number_points > 100000)
                    {
                        cout<<"Too many points! "<<endl;
                        break;
                    }
                }
                cout<<"figure #"<<info.number<<endl;
                cout<<"points: "<<number_points<<endl<<endl;
                if(info.number > 100)
                {
                    cout<<"Too many figures! "<<endl;
                    break;
                }
                if(current_figure->next != NULL)
                    current_figure = current_figure->next;
                else break;
            }

           current_figure = &first_figure;
           while(current_figure->next != NULL)
           {
               number_figures++;
               renew_figure(*current_figure);
               cout<<"figure #"<<number_figures<<endl;
               investigate_figure(current_figure);
               cout<<endl;
               current_figure = current_figure->next;
           }
           info.number = number_figures;
           cout<<endl;
           cout<<"number of figures: "<<info.number<<endl;
           cout<<"circles: "<<info.circles<<endl;
           cout<<"lines: "<<info.lines<<endl;
           cout<<"squares: "<<info.squares<<endl;
           cout<<"triangles: "<<info.triangles<<endl;
           cout<<"not defined: "<<info.not_defined<<endl;
           break;
        default:
            cout<<"Wrong parameter!"<<endl;
            break;
    }



    return 0;
}
