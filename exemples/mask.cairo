// Fond bleu
color(blue());
paint();

// Source rouge
color(red());

// Masque radial : opaque au centre, transparent à l'extérieur
m = newradial(100, 100, 0, 100, 100, 100);

addstop(m, 0, 1, 1, 1, 1);
addstop(m, 1, 1, 1, 1, 0);

// Application du masque
rectangle( 20, 20, 160, 160);
// On clip le rectangle pour délimiter l'application du mask
clip();
//Application du mask : le alpha du pattern vient s'ajouter à la source courante : rouge, laissant plus ou moins apparaitre le fond bleu.
mask(m);