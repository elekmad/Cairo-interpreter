#include "rotate.cairo";
#setoutputsize(1024, 768);

setbgcolor(white());

setfont("arial", "normal", "normal");
setfontsize(50);
setlinewidth(5);
settextcolor(red());


labels=["chakra 1", "chakra 2", "chakra 3", "chakra 4", "charka 5", "chakra 6", "chakra 7", "charka 8", "chakra 9"];
message("Labels :", labels);
center=getoutputsize() / 2;
center=center+concat(0, center[1]/2);

color(black())
{
	rayon=center[1] * 0.8;
	arc(center[0], center[1], rayon, radians(180), 0);
	lineto(center[0]-1*(rayon+getlinewidth()/2), center[1]);
}



nb=getsize(labels);
color(black())
{
	for(i=[1:nb:1])
	{
		rotate_angle=180*(i-1)/nb;
		rotate_angle_text=rotate_angle+180/(2*nb);
		rotate_around(center, radians(rotate_angle_text))
		{
			tsize=textextents(labels[i-1]);
			rmin=tsize[3]/(2*sin(radians(180/(2*nb))));
			dfromcenter=(((rayon-rmin)/2 + rmin)+tsize[2]/2);
			unset(textorg);
			textorg = textorg << (center[0] - dfromcenter) << (center[1] + tsize[3]/2);
			textcenter = textorg + concat(tsize[2]/2, -tsize[3]/2);
			if(rotate_angle_text > 90)
			{
				rotate_around(textcenter, pi())
				{
					moveto(textorg);
					text(labels[i-1]);
				}
			}
			else
			{
				moveto(textorg);
				text(labels[i-1]);
			}
		}
		rotate_around(center, radians(rotate_angle))
		{
			moveto(center);
			lineto(center[0]-1*(rayon+getlinewidth()/2), center[1]);
		}
	}
}
