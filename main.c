// #define BUTTON_BLINK
// #define LIGHT_SCHEDULER
// #define TIME_RAND
// #define KEYPAD
// #define KEYPAD_CONTROL
// #define SEVEN_SEGMENT
// #define KEYPAD_SEVEN_SEGMENT
//#define COLOR_LED
// #define ROTARY_ENCODER
// #define ANALOG
//#define PWM
#define PHASE_ONE
#define PHASE_TWO
#define PHASE_THREE

#include <stdbool.h> // booleans, i.e. true and false
#include <stdio.h>   // sprintf() function
#include <stdlib.h>  // srand() and random() functions

#include "ece198.h"

//Function Declaration
void displayPushButtons(uint16_t pin, char note);
void read_button_input(char user_input[], short capacity);
bool check_passcode(char input[]);
void print_array(char array[], short capacity);
void led_color(char color);
void led_color_delay(char color, short delay);
void change_frequency(uint16_t period, uint16_t const PRESCALE, char color, int delay, TIM_HandleTypeDef pwmTimerInstance);
void generate_melody_nums(char buff, int melody_sequence[], int sequence_length);
void correct_melody_inputted(int melody_num_correct);

int main(void)
{
    HAL_Init(); // initialize the Hardware Abstraction Layer

    // Peripherals (including GPIOs) are disabled by default to save power, so we use the
    // Reset and Clock Control registers to enable the GPIO peripherals that we're using.

    __HAL_RCC_GPIOA_CLK_ENABLE(); // enable port A (for the buttons)
    __HAL_RCC_GPIOB_CLK_ENABLE(); // enable port B (for the RGB LED outputs)
    __HAL_RCC_GPIOC_CLK_ENABLE(); // enable port C (for the on-board blue pushbutton)

    // Initialize the pins to be input, output, alternate function, etc...
    InitializePin(GPIOA, GPIO_PIN_5, GPIO_MODE_INPUT, GPIO_PULLDOWN, 0);                             //White Button
    InitializePin(GPIOA, GPIO_PIN_6, GPIO_MODE_INPUT, GPIO_PULLDOWN, 0);                             //Grey Button
    InitializePin(GPIOA, GPIO_PIN_7, GPIO_MODE_INPUT, GPIO_PULLDOWN, 0);                             //Yellow Button
    InitializePin(GPIOA, GPIO_PIN_9, GPIO_MODE_INPUT, GPIO_PULLDOWN, 0);                             //Red Button
    InitializePin(GPIOA, GPIO_PIN_8, GPIO_MODE_INPUT, GPIO_PULLDOWN, 0);                             //Green Button
    InitializePin(GPIOA, GPIO_PIN_10, GPIO_MODE_INPUT, GPIO_PULLDOWN, 0);                            //Blue Button
    InitializePin(GPIOA, GPIO_PIN_4, GPIO_MODE_INPUT, GPIO_PULLDOWN, 0);                             //White Button
    InitializePin(GPIOB, GPIO_PIN_4, GPIO_MODE_INPUT, GPIO_PULLUP, 0);                               //Passive Buzzer
    InitializePin(GPIOB, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0); //Color led

    SerialSetup(9600);

    uint16_t period_init = 0;
    uint16_t const PRESCALE = 150;

    //Enable Pulse Width Modulation
    __TIM2_CLK_ENABLE();                                                           // enable timer 2
    TIM_HandleTypeDef pwmTimerInstance;                                            // this variable stores an instance of the timer
    InitializePWMTimer(&pwmTimerInstance, TIM2, period_init, PRESCALE);            // initialize the timer instance
    InitializePWMChannel(&pwmTimerInstance, TIM_CHANNEL_1);                        // initialize one channel (can use others for motors, etc)
    InitializePin(GPIOA, GPIO_PIN_0, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF1_TIM2); // connect the buzzer to the timer output

    //Declaration and Initilization of arrays
    uint16_t const period[7] = {204, 182, 162, 153, 136, 121, 108};
    short const color[8] = {7, 3, 5, 4, 1, 2, 6, 0};
    char const passcode[5] = {'E', 'E', 'F', 'G', 'G'};
    short passcode_input[5];

    //Declaration and Initilization of variables
    short const PASSCODE_LENGTH = 5;
    bool correct_passcode = false;
    bool passcode_flag = true;
    short element_counter = 0;
    short delay = 200;
    //int melody_sequence[12];
    //int sequence_length = 12;

#ifdef PHASE_ONE
    led_color(0);
    while (!correct_passcode)
    {

        while (element_counter != PASSCODE_LENGTH)
        {
            if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5))
            {
                change_frequency(period[0], PRESCALE, color[0], delay, pwmTimerInstance);
                SerialPutc('C');

                passcode_input[element_counter] = 'C';
                element_counter++;
            }
            else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6))
            {
                change_frequency(period[1], PRESCALE, color[1], delay, pwmTimerInstance);
                SerialPutc('D');

                passcode_input[element_counter] = 'D';
                element_counter++;
            }
            else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7))
            {
                change_frequency(period[2], PRESCALE, color[2], delay, pwmTimerInstance);
                SerialPutc('E');

                passcode_input[element_counter] = 'E';
                element_counter++;
            }
            else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9))
            {
                change_frequency(period[3], PRESCALE, color[3], delay, pwmTimerInstance);
                SerialPutc('F');

                passcode_input[element_counter] = 'F';
                element_counter++;
            }
            else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8))
            {
                change_frequency(period[4], PRESCALE, color[4], delay, pwmTimerInstance);
                SerialPutc('G');

                passcode_input[element_counter] = 'G';
                element_counter++;
            }
            else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10))
            {
                change_frequency(period[5], PRESCALE, color[5], delay, pwmTimerInstance);
                SerialPutc('A');

                passcode_input[element_counter] = 'A';
                element_counter++;
            }
            else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4))
            {
                change_frequency(period[6], PRESCALE, color[6], delay, pwmTimerInstance);
                SerialPutc('B');

                passcode_input[element_counter] = 'B';
                element_counter++;
            }

            SetPWMDutyCycle(&pwmTimerInstance, TIM_CHANNEL_1, 0);
        }

        short index;

        for (index = 0; index < PASSCODE_LENGTH; index++)
        {
            if (passcode_input[index] != passcode[index])
            {
                passcode_flag = false;
                break;
            }
        }

        if (passcode_flag == false)
        {
            correct_passcode = false;
            passcode_flag = true;
            element_counter = 0;
            SerialPuts("\nFalse\n");

            led_color(0);
            HAL_Delay(1000);
            led_color_delay(4, 3000);
        }
        else
        {
            correct_passcode = true;
            led_color(0);
            HAL_Delay(1000);
            led_color_delay(1, 3000);
        }
    }

    srand(HAL_GetTick());

    SerialPuts("\n");
#endif

#ifdef PHASE_TWO
    char melody_input[12];
    short const MELODY_LENGTH = 12;

    short melody_counter = 0;
    int melody_sequence2[12];
    char melody_sequence_note[12];
    short correct_note = 0;
    //bool correct_note[12] = false;
    delay = 200;
    for (int i = 0; i < MELODY_LENGTH; i++)
    {
        melody_sequence2[i] = random() % 7;

        //char buff[100];
        // sprintf(buff, "The random integer is %lu", melody_sequence2[i], "\n");
        // SerialPuts(buff);
    }
    while (correct_note != MELODY_LENGTH)
    {

       /* for (int i = 0; i < MELODY_LENGTH; i++)
        {
            melody_sequence2[i] = random() % 7;

            //char buff[100];
            // sprintf(buff, "The random integer is %lu", melody_sequence2[i], "\n");
            // SerialPuts(buff);
        }*/
        for (int j = 0; j < MELODY_LENGTH; j++)
        {
            if (melody_sequence2[j] == 0)
            {
                melody_sequence_note[melody_counter] = 'C';
                melody_counter++;
                led_color(color[0]);

                SetPWMDutyCycle(&pwmTimerInstance, TIM_CHANNEL_1, period[0] / 2);
                InitializePWMTimer(&pwmTimerInstance, TIM2, period[0], PRESCALE);
                // HAL_Delay(1000);
            }
            else if (melody_sequence2[j] == 1)
            {
                melody_sequence_note[melody_counter] = 'D';
                melody_counter++;
                led_color(color[1]);

                SetPWMDutyCycle(&pwmTimerInstance, TIM_CHANNEL_1, period[1] / 2);
                InitializePWMTimer(&pwmTimerInstance, TIM2, period[1], PRESCALE);
                // HAL_Delay(1000);
            }
            else if (melody_sequence2[j] == 2)
            {
                melody_sequence_note[melody_counter] = 'E';
                melody_counter++;
                led_color(color[2]);

                SetPWMDutyCycle(&pwmTimerInstance, TIM_CHANNEL_1, period[2] / 2);
                InitializePWMTimer(&pwmTimerInstance, TIM2, period[2], PRESCALE);
                // HAL_Delay(1000);
            }
            else if (melody_sequence2[j] == 3)
            {
                melody_sequence_note[melody_counter] = 'F';
                melody_counter++;
                led_color(color[3]);

                SetPWMDutyCycle(&pwmTimerInstance, TIM_CHANNEL_1, period[3] / 2);
                InitializePWMTimer(&pwmTimerInstance, TIM2, period[3], PRESCALE);
                // HAL_Delay(1000);
            }
            else if (melody_sequence2[j] == 4)
            {
                melody_sequence_note[melody_counter] = 'G';
                melody_counter++;
                led_color(color[4]);

                SetPWMDutyCycle(&pwmTimerInstance, TIM_CHANNEL_1, period[4] / 2);
                InitializePWMTimer(&pwmTimerInstance, TIM2, period[4], PRESCALE);
                // HAL_Delay(100);
            }
            else if (melody_sequence2[j] == 5)
            {
                melody_sequence_note[melody_counter] = 'A';
                melody_counter++;
                led_color(color[5]);

                SetPWMDutyCycle(&pwmTimerInstance, TIM_CHANNEL_1, period[5] / 2);
                InitializePWMTimer(&pwmTimerInstance, TIM2, period[5], PRESCALE);
                // HAL_Delay(100);
            }
            else if (melody_sequence2[j] == 6)
            {
                //SerialPutc('B');

                melody_sequence_note[melody_counter] = 'B';
                melody_counter++;
                led_color(color[6]);

                SetPWMDutyCycle(&pwmTimerInstance, TIM_CHANNEL_1, period[6] / 2);
                InitializePWMTimer(&pwmTimerInstance, TIM2, period[6], PRESCALE);
            }
            SerialPutc(melody_sequence_note[melody_counter - 1]);
            // SetPWMDutyCycle(&pwmTimerInstance, TIM_CHANNEL_1, period[j] / 2);
            // InitializePWMTimer(&pwmTimerInstance, TIM2, period[j], PRESCALE);
            HAL_Delay(1000);
            SetPWMDutyCycle(&pwmTimerInstance, TIM_CHANNEL_1, 0);
            HAL_Delay(300);
            led_color(0);
        }
        led_color(0);
        melody_counter = 0;

        SerialPuts("\n");

        while (melody_counter < MELODY_LENGTH)
        {
            if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5))
            {
                change_frequency(period[0], PRESCALE, color[0], delay, pwmTimerInstance);
                SerialPutc('C');

                melody_input[melody_counter] = 'C';
                melody_counter++;
            }
            else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6))
            {
                change_frequency(period[1], PRESCALE, color[1], delay, pwmTimerInstance);
                SerialPutc('D');

                melody_input[melody_counter] = 'D';
                melody_counter++;
            }
            else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7))
            {
                change_frequency(period[2], PRESCALE, color[2], delay, pwmTimerInstance);
                SerialPutc('E');

                melody_input[melody_counter] = 'E';
                melody_counter++;
            }
            else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9))
            {
                change_frequency(period[3], PRESCALE, color[3], delay, pwmTimerInstance);
                SerialPutc('F');

                melody_input[melody_counter] = 'F';
                melody_counter++;
            }
            else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8))
            {
                change_frequency(period[4], PRESCALE, color[4], delay, pwmTimerInstance);
                SerialPutc('G');

                melody_input[melody_counter] = 'G';
                melody_counter++;
            }
            else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10))
            {
                change_frequency(period[5], PRESCALE, color[5], delay, pwmTimerInstance);
                SerialPutc('A');

                melody_input[melody_counter] = 'A';
                melody_counter++;
            }
            else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4))
            {
                change_frequency(period[6], PRESCALE, color[6], delay, pwmTimerInstance);
                SerialPutc('B');

                melody_input[melody_counter] = 'B';
                melody_counter++;
            }

            led_color(0);
            SetPWMDutyCycle(&pwmTimerInstance, TIM_CHANNEL_1, 0);
        }

        SerialPuts("\nUser input complete\n");

        for (int i = 0; i < MELODY_LENGTH; i++)
        {
            if (melody_input[i] == melody_sequence_note[i])
            {
                correct_note++;
                led_color_delay(1, 1000);
                SerialPutc('#');
                //num_correct++;
            }
            else
            {
                led_color_delay(4, 1000);
                SerialPutc('^');
            }
            HAL_Delay(300);
        }

        if (correct_note == MELODY_LENGTH)
        {
            led_color_delay(2, 5000);
        }
        else
        {
            led_color_delay(4, 5000);
            correct_note = 0;
            melody_counter = 0;
        }
    }

#endif

#ifdef PHASE_THREE
    led_color(0);

    for (int note = 0; note < 7; note++)
    {
        SetPWMDutyCycle(&pwmTimerInstance, TIM_CHANNEL_1, period[note] / 2);
        InitializePWMTimer(&pwmTimerInstance, TIM2, period[note], PRESCALE);
        HAL_Delay(150);
    }

    SetPWMDutyCycle(&pwmTimerInstance, TIM_CHANNEL_1, 0);
    InitializePWMTimer(&pwmTimerInstance, TIM2, 0, PRESCALE);
    while (true)
    {
        led_color(0);
        HAL_Delay(4000);
        led_color_delay(color[1], 2000);
        HAL_Delay(500);
        led_color_delay(color[2], 2000);
        HAL_Delay(500);
        led_color_delay(color[5], 2000);
        HAL_Delay(500);
        led_color_delay(color[3], 2000);
    }

#endif
    //declare and intialize an array to store the passcode input
    //  short user_passcode_input[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    //short passcode[16];

    // note: the on-board pushbutton is fine with the default values (no internal pull-up resistor
    // is required, since there's one on the board)

    // set up for serial communication to the host computer
    // (anything we write to the serial port will appear in the terminal (i.e. serial monitor) in VSCode)

    // as mentioned above, only one of the following code sections will be used
    // (depending on which of the #define statements at the top of this file has been uncommented)

#ifdef BUTTON_BLINK
    // Wait for the user to push the blue button, then blink the LED.

    // wait for button press (active low)
    while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13))
    {
    }

    while (1) // loop forever, blinking the LED
    {
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        HAL_Delay(250); // 250 milliseconds == 1/4 second
    }
#endif

#ifdef LIGHT_SCHEDULER
    // Turn on the LED five seconds after reset, and turn it off again five seconds later.

    while (true)
    { //loop forever
        uint32_t now = HAL_GetTick();
        if (now > 5000 && now < 10000)                  //if program running between 5-10 seconds
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, true); // turn on LED
        else
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, false); // turn off LED
    }
#endif

#ifdef TIME_RAND
    // This illustrates the use of HAL_GetTick() to get the current time,
    // plus the use of random() for random number generation.

    // Note that you must have "#include <stdlib.h>"" at the top of your main.c
    // in order to use the srand() and random() functions.

    // while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13));  // wait for button press
    // srand(HAL_GetTick());  // set the random seed to be the time in ms that it took to press the button
    // if the line above is commented out, your program will get the same sequence of random numbers
    // every time you run it (which may be useful in some cases)

    while (true) // loop forever
    {
        while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13))
            ; // wait for button press

        // Display the time in milliseconds along with a random number.
        // We use the sprintf() function to put the formatted output into a buffer;
        // see https://www.tutorialspoint.com/c_standard_library/c_function_sprintf.htm for more
        // information about this function
        char buff[100];
        sprintf(buff, "Time: %lu ms   Random = %ld\r\n", HAL_GetTick(), random());
        // lu == "long unsigned", ld = "long decimal", where "long" is 32 bit and "decimal" implies signed
        SerialPuts(buff); // transmit the buffer to the host computer's serial monitor in VSCode/PlatformIO

        while (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13))
            ; // wait for button to be released
    }
#endif

#ifdef KEYPAD
    // Read buttons on the keypad and display them on the console.

    // this string contains the symbols on the external keypad
    // (they may be different for different keypads)
    char *keypad_symbols = "123A456B789C*0#D";
    // note that they're numbered from left to right and top to bottom, like reading words on a page

    InitializeKeypad();
    while (true)
    {
        while (ReadKeypad() < 0)
            ;                                     // wait for a valid key
        SerialPutc(keypad_symbols[ReadKeypad()]); // look up its ASCII symbol and send it to the hsot
        while (ReadKeypad() >= 0)
            ; // wait until key is released
    }
#endif

#ifdef KEYPAD_CONTROL
    // Use top-right button on 4x4 keypad (typically 'A') to toggle LED.

    InitializeKeypad();
    while (true)
    {
        while (ReadKeypad() < 0)
            ; // wait for a valid key
        int key = ReadKeypad();
        if (key == 3)                              // top-right key in a 4x4 keypad, usually 'A'
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); // toggle LED on or off
        while (ReadKeypad() >= 0)
            ; // wait until key is released
    }
#endif

#ifdef SEVEN_SEGMENT
    // Display the numbers 0 to 9 inclusive on the 7-segment display, pausing for a second between each one.
    // (remember that the GND connection on the display must go through a 220 ohm current-limiting resistor!)

    Initialize7Segment();
    while (true)
        for (int i = 0; i < 10; ++i)
        {
            Display7Segment(i);
            HAL_Delay(1000); // 1000 milliseconds == 1 second
        }
#endif

#ifdef KEYPAD_SEVEN_SEGMENT
    // Combines the previous two examples, displaying numbers from the keypad on the 7-segment display.

    // this string contains the symbols on the external keypad
    // (they may be different for different keypads)
    char *keypad_symbols = "123A456B789C*0#D";
    // note that they're numbered from left to right and top to bottom, like reading words on a page

    InitializeKeypad();
    Initialize7Segment();
    while (true)
    {
        int key = ReadKeypad();
        if (key >= 0)
            Display7Segment(keypad_symbols[key] - '0'); // tricky code to convert ASCII digit to a number
    }
#endif

#ifdef COLOR_LED
    // Cycle through all 8 possible colors (including off and white) as the on-board button is pressed.
    // This example assumes that the color LED is connected to pins D11, D12 and D13.

    // Remember that each of those three pins must go through a 220 ohm current-limiting resistor!
    // Also remember that the longest pin on the LED should be hooked up to GND.
    InitializePin(GPIOB, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0); // initialize color LED output pins

    // InitializePin(GPIOA, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0);

    //to check if user inputs the correct note in the corresponding pattern outputted and output green on RGB LED
    /* else if (user_passcode_input[element_counter] == passcode[element_counter])
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, !(2 && 0x02); // green (hex 2 == 0010 binary)
        passcode_num_correct++;
    }
    //to output red on RGB LED if user inputs incorrect note
    else
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, !(3x04)); // red   (hex 4 == 0100 binary)
    }
    */

    while (true)
    {
        for (int color = 0; color < 8; ++color)
        {
            // bottom three bits indicate which of the three LEDs should be on (eight possible combinations)
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, !(color & 0x01)); // blue  (hex 1 == 0001 binary)
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, !(color & 0x02)); // green (hex 2 == 0010 binary)
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, !(color & 0x04)); // red   (hex 4 == 0100 binary)

            // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, !(color & 0x01)); // blue  (hex 1 == 0001 binary)
            //HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, !(color & 0x02)); // green (hex 2 == 0010 binary)
            //HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, !(color & 0x04)); // red   (hex 4 == 0100 binary)

            while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13))
                ; // wait for button press
            SerialPuts("Led Button pressed");
            while (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13))
                ; // wait for button release
        }
    }
#endif

#ifdef ROTARY_ENCODER
    // Read values from the rotary encoder and update a count, which is displayed in the console.

    InitializePin(GPIOB, GPIO_PIN_5, GPIO_MODE_INPUT, GPIO_PULLUP, 0);  // initialize CLK pin
    InitializePin(GPIOB, GPIO_PIN_4, GPIO_MODE_INPUT, GPIO_PULLUP, 0);  // initialize DT pin
    InitializePin(GPIOB, GPIO_PIN_10, GPIO_MODE_INPUT, GPIO_PULLUP, 0); // initialize SW pin

    bool previousClk = false; // needed by ReadEncoder() to store the previous state of the CLK pin
    int count = 0;            // this gets incremented or decremented as we rotate the encoder

    while (true)
    {
        int delta = ReadEncoder(GPIOB, GPIO_PIN_5, GPIOB, GPIO_PIN_4, &previousClk); // update the count by -1, 0 or +1
        if (delta != 0)
        {
            count += delta;
            char buff[100];
            sprintf(buff, "%d  \r", count);
            SerialPuts(buff);
        }
        bool sw = !HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10); // read the push-switch on the encoder (active low, so we invert it using !)
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, sw);        // turn on LED when encoder switch is pushed in
    }
#endif

#ifdef ANALOG
    // Use the ADC (Analog to Digital Converter) to read voltage values from two pins.

    __HAL_RCC_ADC1_CLK_ENABLE();       // enable ADC 1
    ADC_HandleTypeDef adcInstance;     // this variable stores an instance of the ADC
    InitializeADC(&adcInstance, ADC1); // initialize the ADC instance
    // Enables the input pins
    // (on this board, pin A0 is connected to channel 0 of ADC1, and A1 is connected to channel 1 of ADC1)
    InitializePin(GPIOA, GPIO_PIN_0 | GPIO_PIN_1, GPIO_MODE_ANALOG, GPIO_NOPULL, 0);
    while (true)
    {
        // read the ADC values (0 -> 0V, 2^12 -> 3.3V)
        uint16_t raw0 = ReadADC(&adcInstance, ADC_CHANNEL_0);
        uint16_t raw1 = ReadADC(&adcInstance, ADC_CHANNEL_1);

        // print the ADC values
        char buff[100];
        sprintf(buff, "Channel0: %hu, Channel1: %hu\r\n", raw0, raw1); // hu == "unsigned short" (16 bit)
        SerialPuts(buff);
    }
#endif

#ifdef PWM
    // Use Pulse Width Modulation to fade the LED in and out.
    uint16_t period = 204, prescale = 150; //clock/num

    //period: cycle before reset
    //increase period frequency down
    //prescale increase

    __TIM2_CLK_ENABLE();                                           // enable timer 2
    TIM_HandleTypeDef pwmTimerInstance;                            // this variable stores an instance of the timer
    InitializePWMTimer(&pwmTimerInstance, TIM2, period, prescale); // initialize the timer instance
    InitializePWMChannel(&pwmTimerInstance, TIM_CHANNEL_1);        // initialize one channel (can use others for motors, etc)
    SerialPuts("Buzzer Reached");
    InitializePin(GPIOA, GPIO_PIN_0, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF1_TIM2); // connect the buzzer to the timer output
                                                                                   // SetPWMDutyCycle(&pwmTimerInstance, TIM_CHANNEL_1, period / 2);

    while (true)
    {
        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5))
        {
            period = 204; //C4
            SetPWMDutyCycle(&pwmTimerInstance, TIM_CHANNEL_1, period / 2);
            InitializePWMTimer(&pwmTimerInstance, TIM2, period, prescale);
            SerialPutc('C');
            led_color(7);
            HAL_Delay(100);
        }
        else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6))
        {
            period = 182;
            SetPWMDutyCycle(&pwmTimerInstance, TIM_CHANNEL_1, period / 2);
            InitializePWMTimer(&pwmTimerInstance, TIM2, period, prescale);
            SerialPutc('D');
            led_color(3);
            HAL_Delay(100);
        }
        else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7))
        {
            period = 162; //E4
            SetPWMDutyCycle(&pwmTimerInstance, TIM_CHANNEL_1, period / 2);
            InitializePWMTimer(&pwmTimerInstance, TIM2, period, prescale);
            SerialPutc('E');
            led_color(5);
            HAL_Delay(100);
        }
        else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9))
        {
            period = 153; //F4
            SetPWMDutyCycle(&pwmTimerInstance, TIM_CHANNEL_1, period / 2);
            InitializePWMTimer(&pwmTimerInstance, TIM2, period, prescale);
            SerialPutc('F');
            led_color(4);
            HAL_Delay(100);
        }
        else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8))
        {
            period = 136; //G4
            SetPWMDutyCycle(&pwmTimerInstance, TIM_CHANNEL_1, period / 2);
            InitializePWMTimer(&pwmTimerInstance, TIM2, period, prescale);
            SerialPutc('G');
            led_color(1);
            HAL_Delay(100);
        }
        else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10))
        {
            period = 121; //A4
            SetPWMDutyCycle(&pwmTimerInstance, TIM_CHANNEL_1, period / 2);
            InitializePWMTimer(&pwmTimerInstance, TIM2, period, prescale);
            SerialPutc('A');
            led_color(2);
            HAL_Delay(100);
        }
        else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4))
        {
            period = 108; //B4
            SetPWMDutyCycle(&pwmTimerInstance, TIM_CHANNEL_1, period / 2);
            InitializePWMTimer(&pwmTimerInstance, TIM2, period, prescale);
            SerialPutc('B');
            led_color(6);
            HAL_Delay(100);
        }
        SetPWMDutyCycle(&pwmTimerInstance, TIM_CHANNEL_1, 0);
    }
/*while (true)
    {
        // fade the LED in by slowly increasing the duty cycle
        for (uint32_t i = 0; i < period; ++i)
        {
            SetPWMDutyCycle(&pwmTimerInstance, TIM_CHANNEL_1, i);
            HAL_Delay(5);
        }
        // fade the LED out by slowly decreasing the duty cycle
        for (uint32_t i = period; i > 0; --i)
        {
            SetPWMDutyCycle(&pwmTimerInstance, TIM_CHANNEL_1, i);
            HAL_Delay(5);
        }
        return 0;
    }*/
#endif
}

// This function is called by the HAL once every millisecond
void SysTick_Handler(void)
{
    HAL_IncTick(); // tell HAL that a new tick has happened
    // we can do other things in here too if we need to, but be careful
}

void displayPushButtons(uint16_t pin, char note)
{
    if (HAL_GPIO_ReadPin(GPIOA, pin))
    {
        SerialPutc(note);
    }
    else
    {
        SerialPutc('_');
    }
}

void print_array(char array[], short capacity)
{

    for (int i = 0; i < capacity; ++i)
    {
        SerialPutc(array[i]);
    }
}

void read_button_input(char user_input[], short capacity)
{
    SerialPuts("Read button input method entered");
    short element_counter = 0;

    while (true)
    {
        if (element_counter == capacity)
        {
            SerialPuts("Counter: 12");
            break;
        }
        else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5))
        { //Reads the white button
            user_input[element_counter] = 'A';
            // SerialPutc('A');
            HAL_Delay(500);
            element_counter++;
        }
        else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6))
        { //Reads the grey button
            user_input[element_counter] = 'B';
            //   SerialPutc('B');
            HAL_Delay(500);
            element_counter++;
        }
        else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7))
        { //Reads the yellow button
            user_input[element_counter] = 'C';
            //  SerialPutc('C');
            HAL_Delay(500);
            element_counter++;
        }
        else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9))
        { //Reads the red button
            user_input[element_counter] = 'D';
            // SerialPutc('D');
            HAL_Delay(500);
            element_counter++;
        }
        else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8))
        { //Reads the green button
            user_input[element_counter] = 'E';
            // SerialPutc('E');
            HAL_Delay(500);
            element_counter++;
        }
        else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10))
        { //Reads the blue button
            user_input[element_counter] = 'F';
            //SerialPutc('F');
            HAL_Delay(500);
            element_counter++;
        }
        else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4))
        { //Reads the black button
            user_input[element_counter] = 'A';
            //SerialPutc('G');
            HAL_Delay(500);
            element_counter++;
        }
    }
}

void led_color(char color)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, !(color & 0x01)); // blue  (hex 1 == 0001 binary)
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, !(color & 0x02)); // green (hex 2 == 0010 binary)
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, !(color & 0x04)); //red (hex 3 == 0100 binary)
}

void led_color_delay(char color, short delay)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, !(color & 0x01)); // blue  (hex 1 == 0001 binary)
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, !(color & 0x02)); // green (hex 2 == 0010 binary)
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, !(color & 0x04)); //red (hex 3 == 0100 binary)

    HAL_Delay(delay);

    led_color(0);
}

/*bool check_passcode(char passcode_input[]){
    short const capacity = 5;
    char const PASSCODE[capacity] ={'E','E','F','G','G'};

    for(short note = 0; note<capacity; ++note){
        if(passcode_input[note] != PASSCODE[note]){
            return false;
        }
    }

    return true;
}*/

void change_frequency(uint16_t period, uint16_t const PRESCALE, char color, int delay, TIM_HandleTypeDef pwmTimerInstance)
{
    SetPWMDutyCycle(&pwmTimerInstance, TIM_CHANNEL_1, period / 2);
    InitializePWMTimer(&pwmTimerInstance, TIM2, period, PRESCALE);
    led_color(color);
    HAL_Delay(delay);
}

bool check_passcode(char input[])
{
    char passcode[5] = {'E', 'E', 'F', 'G', 'G'};

    for (int i = 0; i < 5; i++)
    {
        if (input[i] != passcode[i])
        {
            return false;
        }
    }

    return true;
}

void correct_melody_inputted(int melody_num_correct)
{
    //to check if user inputs all 12 notes of the melody correctly
    if (melody_num_correct == 12)
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, !(1 && 0x01)); // blue  (hex 1 == 0001 binary)
    }
    else
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, !(4 & 0x04)); //red (hex 3 == 0100 binary)
    }
}

void generate_melody_nums(char buff, int melody_sequence[], int sequence_length)
{
    int random_num = (int)buff;
    int random_num_value;
    melody_sequence[12];

    for (int i = 0; i < sequence_length; i++)
    {
        melody_sequence[i] = random_num % 7;
        //random_num_value = random_num%7;
        sprintf("%lu", random_num);
        SerialPuts("\n");
        SerialPuts(melody_sequence[i]);
        SerialPuts("\n");
        random_num = random_num / 7;
    }
}