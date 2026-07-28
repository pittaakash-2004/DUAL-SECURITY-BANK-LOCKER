🔐 DUAL-SECURITY BANK LOCKER

📖 Overview
    
The Dual-Security Bank Locker is an embedded security system designed to provide enhanced protection for bank lockers using two-factor authentication. The system combines password verification and fingerprint authentication to ensure that only authorized users can access the locker.
Built around the LPC2148 ARM7 microcontroller, the project interfaces with a fingerprint module, keypad, EEPROM, LCD, and DC motor to provide a secure, reliable, and user-friendly locker access system.

🎯 Aim

  To develop a secure embedded bank locker system that authenticates users through two levels of security:
  1)Password Verification
  2)fingerprint Authentication
  Only after successful verification of both authentication methods will the locker be unlocked.

  📌 Block Diagram

<img width="673" height="874" alt="image" src="https://github.com/user-attachments/assets/71f57d0b-2d8c-4352-9f83-9c19f8e323ef" />

✨ Features

- Dual-factor authentication (Password + Fingerprint)
- Secure password storage in EEPROM
- Fingerprint enrollment and verification
- LCD-based user interface
- Automatic locker opening and closing
- Password update functionality
- Fingerprint enrollment and deletion
- Unauthorized access protection

 🛠 Hardware Requirements

- LPC2148 ARM7 Microcontroller
- R305 Fingerprint Module
- 4×4 Matrix Keypad
- 16×2 LCD Display
- AT24C256 EEPROM
- L293D Motor Driver
- DC Motor
- Push Button (External Interrupt)
- USB-to-UART Converter

 💻 Software Requirements

- Embedded C
- Keil µVision IDE
- Flash Magic
- UART Driver
- I2C Driver
- 
 🏗 System Architecture

The system consists of the following modules:

🔹 LPC2148 ARM7 Controller
- Controls all peripherals
- Executes authentication logic
- Controls locker operation

 🔹 Keypad Module
- User ID entry
- Password entry
- Menu selection

🔹 EEPROM Module
- Stores password securely
- Allows password updates

🔹 Fingerprint Module
- Fingerprint enrollment
- Fingerprint search
- Fingerprint deletion

 🔹 LCD Display
- Displays user instructions
- Authentication status
- Success and failure messages

 🔹 Motor Driver & DC Motor
- Opens locker after successful authentication
- Automatically closes locker after a delay

 🔄 Working Principle

1. System initializes all peripherals.
2. User enters User ID.
3. User enters password using the keypad.
4. Password is compared with the password stored in EEPROM.
5. If the password matches, fingerprint verification starts.
6. The fingerprint module searches for a matching fingerprint.
7. If both authentications are successful:
   - DC motor rotates forward to open the locker.
   - After a delay, the motor rotates in reverse to close the locker.
8. If authentication fails, access is denied and the system waits for the next user.

 📡 Communication Protocols

 UART
Used for communication between:
- LPC2148
- R305 Fingerprint Module

  I2C
Used for communication between:
- LPC2148
- AT24C256 EEPROM

 📁 Project Structure
DUAL-SECURITY-BANK-LOCKER/

├── main.c

├── lcd.c ├── lcd.h
    
├── kpm.c ├── kpm.h

├── uart0.c ├── uart0.h 

├── ext_int.c ├── ext_int.h

├── i2c.c ├── i2c.h

├── i2c_eeprom.c ├── i2c_eeprom.h

├── R305.c ├── R305.h

├── delay.c├── delay.h

├── device.c├── device.h

├── README.md

 🚀 Implementation Steps

1. Interface LCD and display messages.
2. Interface the keypad for password entry.
3. Read and write password to EEPROM.
4. Establish UART communication with the fingerprint module.
5. Test fingerprint enrollment and search.
6. Implement password verification.
7. Implement fingerprint verification.
8. Control DC motor using L293D.
9. Add administrator functions for password and fingerprint management.
10. Integrate all modules and test the complete system.

 🎓 Learning Outcomes

Through this project, the following concepts were implemented and learned:

- Embedded C Programming
- LPC2148 ARM7 Programming
- UART Communication
- I2C Communication
- EEPROM Interfacing
- Fingerprint Module Interfacing
- LCD Interfacing
- Matrix Keypad Programming
- Motor Driver Control
- Interrupt Handling
- Real-Time Embedded System Design

 🔮 Future Enhancements

- Iris (Eye) Recognition Authentication
- RFID-Based User Identification
- OTP Verification using GSM
- Mobile Application Control
- Wi-Fi/IoT-Based Remote Monitoring
- Cloud-Based Access Logs
- Face Recognition
- Emergency Alarm System

 📌 Applications

- Bank Safe Deposit Lockers
- Jewelry Shops
- Home Security Lockers
- Office Document Storage
- Hospitals
- Government Offices
- Research Laboratories

 👨‍💻 Author

AKASH PITTA

Bachelor of Technology (Electronics and Communication Engineering).
