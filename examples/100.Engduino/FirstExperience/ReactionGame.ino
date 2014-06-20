
// Called when the user made a mistake. Flashes all the leds
// red a couple of times as a punishment, then gives a run of
// colurful LEDs as a reward for how well the user did.
// 
void oops(int success) {
  colour c[16] = {OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF};

  // Punishment - flash LEDs red
  //
  for (int i = 0; i < 2; i++) {
    EngduinoLEDs.setAll(RED, brightness);
    delay(500);
    EngduinoLEDs.setAll(OFF, brightness);
    delay(500);    
  }
}

void ReactionGame() {
    while (true) {
    // Pick an LED and the colour for that LED at random
    // Colour is chosen from primary and secondary colours
    // plus white. Dark is, clearly disallowed.
    //
    uint8_t r   = random(2);
    uint8_t g   = random(2);
    uint8_t b   = random(2);
    uint8_t led = random(1,17);
    
    // Choose how long we should have the LED on and off - has a random component.
    //
    long ontime  = minontime  + random(minontime);
    long offtime = minofftime + random(minofftime);
    long  onAt;  // Time the LED went on at - i.e. the time from which the user should react
    long offAt;  // When we switched the LED off
    
    bool mustPush = (r==0 && g==0 && b==1);  // If it's blue, the user must push
    bool   pushed = false;

    if (mustPush)
      Serial.println("PUSH!");
    
    // Set the chosen LED to the chosen colour
    //
    EngduinoLEDs.setLED(led, r*MAX_BRIGHTNESS, g*MAX_BRIGHTNESS, b*MAX_BRIGHTNESS);
    
    // And let's see if the button is pressed before the LED goes out
    //
    onAt = millis();
    delay(ontime);
    pushed = EngduinoButton.wasPressed();
    if (pushed && !mustPush) {
      // We're definitely not OK if we pushed and we shouldn't have
      // We might still have a little time left to push if we should
      oops(constrain(round(500/minontime), 1, 16));
      
      // Reset
      minontime = 500;
      continue;
    }
    
    // Now turn the LED out and wait a while before showing the next
    // We need to continue to check to see if the button was pressed - and
    // if so, we need to test whether it was pressed in time.
    EngduinoLEDs.setLED(led, OFF);
   
    offAt = millis();
    while (millis() - offAt < offtime) {
      if (EngduinoButton.wasPressed()) {
        if (mustPush)
          pushed |= ((millis() - onAt) < OKTime);    // If we're too late, we're too late.
        else {
          pushed = true;
          break;
        }
      }
    }
    if ( !((pushed && mustPush) || (!pushed && !mustPush)) ) {
      oops(constrain(round(500/minontime), 1, 16));

      // Reset
      minontime = 500;
      continue;
    }
    
    if (pushed)                // Reduce the on time gradually: more if the button was
      minontime *= 0.8;        // successfully pushed than successfully not pushed.
    else
      minontime *= 0.99;
  }
}


