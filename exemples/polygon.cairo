setlinewidth(2);
setautofill();
a=[10, 10, 10, 20, 20, 20, 20, 10];
polygon(a);
stroke();

nf=10;
color(red()+blue())
{
	for(i=[0:nf])
	{
		angle=2*pi()*i/nf;
		x=100+cos(angle)*30;
		y=100+sin(angle)*30;
		coords = coords << x;
		coords = coords << y;
	}

	polygon(coords);
}

unset(coords);

color(red()+green())
{
	for(i=[0:nf])
	{
		angle=2*pi()*i/nf;
		x=100+rand(30)-15+cos(angle)*60;
		y=100+rand(30)-15+sin(angle)*60;
		coords = coords << x;
		coords = coords << y;
	}

	polygon(coords);
	stroke();
}