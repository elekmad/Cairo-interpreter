#include "rotate.cairo";
#setoutputsize(600, 600);

setlinewidth(2);

center=getoutputsize()/2;
rhorloge=center[0]*0.8;

GrayonL=rhorloge/4;
GrayonH=GrayonL/8;

prayonL=GrayonL*3/4*3/4;
prayonH=prayonL/8;
aHL=rhorloge*0.7; //Aiguille des Heures
aHH=aHL/10;

aML=aHL*0.8; // Aiguille des minutes
aMH=aML/10;

aSL=aML*0.5; //Aiguille des secondes
aSH=aSL/10;

for(i=[0:3])
{
    rotate_around(center, i*pi()/2)
    {
        rectangle(center[0]-rhorloge, center[1]-GrayonH/2, GrayonL, GrayonH);
        rotate_around(center, pi()/6)rectangle(center[0]-rhorloge, center[1]-prayonH/2, prayonL, prayonH);
        rotate_around(center, 2*pi()/6)rectangle(center[0]-rhorloge, center[1]-prayonH/2, prayonL, prayonH);
    }
}
fill();

Now=getlocaltime(gettimeinsecs());
NowH=Now[2];
NowM=Now[1];
NowS=Now[0];
color(black())rotate_around(center, pi()/2+pi()/6*NowH)rectangle(center[0]-aHL, center[1]-aHH/2, aHL, aHH);
color(blue())rotate_around(center, pi()/2+2*pi()/60*NowM)rectangle(center[0]-aML, center[1]-aMH/2, aML, aMH);
color(red())rotate_around(center, pi()/2+2*pi()/60*NowS)rectangle(center[0]-aSL, center[1]-aSH/2, aSL, aSH);

