#include "KEY_INPUT.H"
#include <stdio.h>
#include <go32.h>
#include <dpmi.h>
#include <dos.h>

bool keyStates [NUM_CODES];
_go32_dpmi_seginfo old_keydriver,new_keydriver;

bool checkKeyState ( int scanCode){
    return keyStates[scanCode];
}

static void KeyDriver(){

    //get the key from the queue
    char key = inportb(0x60);
    char temp;
    //remove the key from the queue
    outportb(0x61,(temp = inportb(0x61)) | 0x80);
    outportb(0x61,temp);

    if(key & 0x80){ //was released
        key &= 0x7f;
        keyStates[key] = false;
    }else{ // was pressed
        keyStates[key] = true;
    }

    outportb(0x20,0x20);
}

void installKeyDriver(){
    _go32_dpmi_get_protected_mode_interrupt_vector(9,&old_keydriver);

    new_keydriver.pm_offset = (int) KeyDriver;
    new_keydriver.pm_selector = _go32_my_cs();

    //create an assembly routine (like 'interrupt' in Turbo C)
    _go32_dpmi_allocate_iret_wrapper(&new_keydriver);

    //set the interrupt handler to KeyDriver method
    _go32_dpmi_set_protected_mode_interrupt_vector(9,&new_keydriver);
}

void uninstallKeyDriver(){
    _go32_dpmi_set_protected_mode_interrupt_vector(9,&old_keydriver);
    _go32_dpmi_free_iret_wrapper(&new_keydriver);
}
