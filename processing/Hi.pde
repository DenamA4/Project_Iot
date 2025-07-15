import processing.serial.*; // imports library for serial communication
import java.awt.event.KeyEvent; // imports library for reading the data from the serial port
import java.io.IOException;
Serial myPort; // defines Object Serial
// defubes variables
String angle="";
String distance="";
String data="";
String noObject;
float pixsDistance;
int iAngle, iDistance;
int index1=0;
int index2=0;
float x,y;

//PFont orcFont;
void setup() {
  size (1920, 1080);
  smooth();
  myPort = new Serial(this,"COM4", 115200); // starts the serial communication COM4
  myPort.bufferUntil('.'); // reads the data from the serial port up to the character '.'. So actually it reads this: angle,distance.
  //orcFont = loadFont("Arial-Black-20.vlw");
//fullScreen();
}

void draw() {
  
  fill(98,245,31);
  //textFont(orcFont);
  //simulating motion blur and slow fade of the moving line
  noStroke();
  fill(0,4); 
  rect(0, 0, width, 1010);   //rect(0, 1010, width, 1080);
  
  fill(98,245,31); // green color
  //calls the functions for drawing the radar
  drawRadar(); 
  drawLine();
  drawObject();
  drawText();
}

void serialEvent (Serial myPort) { // starts reading data from the Serial Port
  // reads the data from the Serial Port up to the character '.' and puts it into the String variable "data".
  data = myPort.readStringUntil('.');
  if(data != null) {
    data = data.substring(0,data.length()-1);
  
    index1 = data.indexOf(","); // find the character ',' and puts it into the variable "index1"
    angle= data.substring(0, index1); // read the data from position "0" to position of the variable index1 or thats the value of the angle the Arduino Board sent into the Serial Port
    distance= data.substring(index1+1, data.length()); // read the data from position "index1" to the end of the data pr thats the value of the distance
  
    // converts the String variables into Integer
    iAngle = int(angle);
    iDistance = int(distance);
  }
}
void drawRadar() {
  pushMatrix();
  translate(960,500); // moves the starting coordinats to new location
  noFill();
  strokeWeight(2);
  stroke(98,245,31);
  // draws the circle lines
  for (int r = 200; r <= 1000; r += 200) {
    circle(0, 0, r);
  }
 
   // draws the angle lines
  for (int angle = 30; angle <= 330; angle += 30) {
    line(0, 0, -500 * cos(radians(angle)), -500 * sin(radians(angle)));
  }
  
  line(-580*cos(radians(30)),0,500,0);
  popMatrix();
}

void drawObject() {
  pushMatrix();
  translate(960, 500); // moves the starting coordinates to a new location
  strokeWeight(15);
  stroke(255, 10, 10); // red color

  // Calculate pixsDistance based on the distance (up to 1200 cm)
  // Map the distance from cm to pixels directly, where max 1200 cm corresponds to a maximum radius in pixels (adjust as needed)
  pixsDistance = map((float)(iDistance / 2 + 1), 0, 1200, 0, 1000); // Assuming 1200 pixels is the maximum visual distance on the radar

  // Only draw the object if the distance is valid
  if (iDistance <= 1200 && iDistance > 0) {
    // Draw the object according to the angle and the distance in pixels
    x = pixsDistance * cos(radians(iAngle * -1));
    y = -pixsDistance * sin(radians(iAngle * -1));
    point(x, y);
    pushMatrix();
    translate(960, 500);
    text("Coordinates of red dot (object):", 500, -420);
    text("(" + (int)x + "," + " " + (int)y + ")", 650, -300);
    popMatrix();
  }
  popMatrix();
}

void drawLine() {
  pushMatrix();
  strokeWeight(5);
  stroke(30,250,60);
  translate(960,500); // moves the starting coordinats to new location
  line(0,0,500*cos(radians(iAngle*-1)),-500*sin(radians(iAngle*-1))); // draws the line according to the angle
  popMatrix();
}

void drawText() { // draws the texts on the screen
  
  pushMatrix();
  if(iDistance>1200) noObject = "";
  fill(0,0,0);
  noStroke();
  rect(0, 1010, width, 1080);
  fill(98,245,31);
  textSize(15);
  text("240cm",1060,490);
  text("480cm",1160,490);
  text("720cm",1260,490);
  text("960cm",1360,490);
  text("1200cm",1460,490);
  textSize(30);
  text("" + noObject, 240, 1035);
  text("Angle: " + iAngle +" °", 1050, 1035);
  text("Distance: ", 1380, 1035);
  if(iDistance<1200) {
  text("        " + iDistance +" cm", 1500, 1035);
  }
  popMatrix(); 
}
