void main()
{
    unsigned char first = 0x00, second = 0x00, output = 0x00;
    unsigned char prev_first = 0x00;  // Track previous state of 'first'
    int pressed = 0, i = 0;

    // Configure PORTB as output (for 7-segment display)
    TRISB = 0x00;
    PORTB = 0x00;

    // Set up keypad rows and columns
    TRISC.F0 = 1; TRISC.F1 = 1; TRISC.F2 = 1;
    TRISC.F3 = 0; TRISC.F4 = 0; TRISC.F5 = 0; TRISC.F6 = 0;
    PORTC.F3 = 1; PORTC.F4 = 1; PORTC.F5 = 1; PORTC.F6 = 1;

    // Countdown button setup
    TRISD.F1 = 1;  // Set PORTD.F1 as input (button)
    PORTD.F1 = 0;  // Initially low (not pressed)

    // LED Setup
    TRISA = 0x00;
    PORTA = 0x00;

    while (1)  // Infinite loop
    {
        // === Scan Keypad ===
        PORTC.F3 = 1; PORTC.F4 = 0; PORTC.F5 = 0; PORTC.F6 = 0;
        if (PORTC.F0 == 1) first = 0x03;
        if (PORTC.F1 == 1) first = 0x02;
        if (PORTC.F2 == 1) first = 0x01;
        DELAY_MS(20);

        PORTC.F3 = 0; PORTC.F4 = 1;
        if (PORTC.F0 == 1) first = 0x06;
        if (PORTC.F1 == 1) first = 0x05;
        if (PORTC.F2 == 1) first = 0x04;
        DELAY_MS(20);

        PORTC.F4 = 0; PORTC.F5 = 1;
        if (PORTC.F0 == 1) first = 0x09;
        if (PORTC.F1 == 1) first = 0x08;
        if (PORTC.F2 == 1) first = 0x07;
        DELAY_MS(20);

        PORTC.F5 = 0; PORTC.F6 = 1;
        if (PORTC.F1 == 1) first = 0x00;
        DELAY_MS(10);

        // Check if input changed
        if (first != prev_first) {
            second = prev_first;
            prev_first = first;
        }

        // Only update output when countdown is NOT running
        if (pressed == 0) {
            output = (second << 4) | first;
        }

        PORTB = output;  // Display on 7-segment

        // === Countdown Button Logic ===
        if (PORTD.F1 == 1) {
            DELAY_MS(500);  // Debounce delay
            pressed = !pressed;  // Toggle start/stop
        }

        if (pressed == 1)  // Countdown active
        {
            while (output > 0)
            {
                PORTA.F2 = 00; //Blue LED on Indicating Countdown reachesnot  Zero
                if (PORTD.F1 == 1) {  // Pause if button is pressed
                    DELAY_MS(500);
                    pressed = 0;
                    break;
                }

                // Decrement output while avoiding HEX letters
                if (output > 0) {
                    PORTA.F0 = 1; // Countdown is in progress
                    if (PORTB.F0 == 0 && PORTB.F1 == 0 && PORTB.F2 == 0 && PORTB.F3 == 0) {
                        output -= 7;
                    } else {
                        output--;
                    }
                    PORTB = output;
                }

                // Fast button detection for pause
                for (i = 0; i < 10; i++) {
                    DELAY_MS(50);
                    if (PORTD.F1 == 1) {
                        DELAY_MS(500);
                        pressed = 0;
                        PORTA.F0 = 0; // Countdown is not in progress
                        PORTA.F1 = 1; //Red LED on indicating countdown is paused

                        // Pause loop until button is pressed again
                        while (pressed == 0) {
                            if (PORTD.F1 == 1) {
                                DELAY_MS(500);
                                pressed = 1;  // Resume
                                PORTA.F1 = 0; //Red LED on indicating countdown is resume
                            }
                        }
                        break;
                    }
                }
            }
            PORTA.F0 = 0; // Countdown is not in progress
            PORTA.F2 = 1; //Blue LED on Indicating Countdown reaches Zero
        }
    }
}
