/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include <iostream>

// set pins
#define PIN_L_WHEEL       1
#define PIN_R_WHEEL       0
#define PIN_L_CLAW        2
#define PIN_R_CLAW        3
#define PIN_ARM           4

#define PIN_SERVO1        5
#define PIN_SERVO2        6

#define B_ARM_UP          2
#define B_ARM_DOWN        1
#define B_RESETCAM        7

#define POSITION          1

#define AUTO_DRIVE_TIME   2.0       // seconds
#define AUTO_DRIVE_SPEED  0.7
#define AUTO_TURN_TIME    0.5     // seconds
#define AUTO_FINAL_TIME   1 //seconds
#define AUTO_TURN_SPEED   0.6

#define AUTO_DRIVE_TIME_R   0.5       // seconds
#define AUTO_TURN_TIME_R    0.5     // seconds
#define AUTO_FINAL_TIME_R   2 //seconds

class Robot : public frc::IterativeRobot {

	/*******************
	 * CREATE COMPONENTS
	 *******************/

	// set controller
	frc::Joystick *m_stick1 = new Joystick(0);
	frc::Joystick *m_stick2 = new Joystick(1);

	// set wheel controllers
	frc::Spark m_lWheelMotor { PIN_L_WHEEL };
	frc::Spark m_rWheelMotor { PIN_R_WHEEL };
	frc::DifferentialDrive m_robotDrive { m_lWheelMotor, m_rWheelMotor };

	// set claw controllers
	frc::Spark m_lClawMotor { PIN_L_CLAW };
	frc::Spark m_rClawMotor { PIN_R_CLAW };
	frc::DifferentialDrive m_clawDrive { m_lClawMotor, m_rClawMotor };

	frc::Servo *sv_1 = new Servo(PIN_SERVO1);
	frc::Servo *sv_2 = new Servo(PIN_SERVO2);

	// set arm controller
	frc::Jaguar *m_armMotor = new Jaguar(PIN_ARM);

	// set the autonomous mode timer
	frc::Timer *autoTimer = new Timer();


	// get the game data
	std::string gameData;
	int position = 3;
	char switchSide;
	double servoX = 0.5;
	double servoY = 0.5;
public:

	Robot() {}

	// runs on robot initialization
	void RobotInit() {
		//does nothing? Hopefully you can use this to set the starting position
		//position = SmartDashboard::GetNumber("Position", 1);
		// set up camera
		//CameraServer::GetInstance()->StartAutomaticCapture();

	}

	void AutonomousInit() {
		autoTimer->Start();
		autoTimer->Reset();

		// get the game data
		gameData = frc::DriverStation::GetInstance().GetGameSpecificMessage();

		// set the switch side (X if not detected)
		if (gameData.length() > 0) {
			switchSide = gameData[0];
		} else {
			switchSide = 'X';
		}

		SmartDashboard::PutString("Sides (closest to farthest)", gameData);
		SmartDashboard::PutBoolean("Our switch colour is on the LEFT?",switchSide == 'L');
		if (switchSide == 'l') {
			switchSide = 'L';
		}
		if (switchSide == 'r') {
			switchSide = 'R';
		}
	}
	void AutonomousPeriodic() {
		auto_move();
	}


	// loops on teleop mode
	void TeleopPeriodic() {

		// drive the robot with the first joystick
		m_robotDrive.ArcadeDrive(-m_stick1->GetY(), m_stick1->GetX() * 0.7);

		// drive the claws with the second joystick
		//m_clawDrive.ArcadeDrive(m_stick2.GetY(), m_stick2.GetX());
		m_clawDrive.ArcadeDrive(m_stick2->GetZ(), m_stick2->GetX());
		int pos = m_stick1->GetPOV(0);
		//std::cout << pos << std::endl;
		if (pos != -1) {
			switch(pos) {
				case 90:
					servoX = 0.65;
					break;
				case 180:
					servoY = 0.65;
					break;
				case 270:
					servoX = 0.35;
					break;
				case 0:
					servoY = 0.35;
					break;
				default:
					break;
			}
		}
		if (m_stick1->GetRawButton(B_RESETCAM)) {
			servoX = 0.5;
			servoY = 0.5;
		}
		sv_1->Set(servoX);
		sv_2->Set(servoY);

		// grab the claw with the joystick trigger
		if (m_stick2->GetRawButton(B_ARM_UP)) {
			m_armMotor->SetSpeed(-0.6);
		}
		// eject the claw with the joystick thumb button
		else if (m_stick2->GetRawButton(B_ARM_DOWN)) {
			m_armMotor->SetSpeed(0.4);
		}
		// stop the claw if the claw isn't moving
		else {
			m_armMotor->SetSpeed(-0.1);
		}
	}

	/***********************/
	/* AUTONOMOUS COMMANDS */
	/***********************/
	void auto_move() {
		SmartDashboard::PutNumber("Seconds Remaining",15-autoTimer->Get());
		m_armMotor->SetSpeed(-0.5);

		// drive time
		if (autoTimer->Get() < AUTO_DRIVE_TIME) {
			m_robotDrive.ArcadeDrive(AUTO_DRIVE_SPEED, 0); //10.72.67.2 rio ip
		}
		// turn time
		else if (autoTimer->Get() < (AUTO_TURN_TIME + AUTO_DRIVE_TIME)) {
			// Left side
			if (switchSide == 'L') {
				//Go for it
				m_robotDrive.ArcadeDrive(-AUTO_TURN_SPEED, AUTO_TURN_SPEED);
			}
//			if (POSITION == 3 && switchSide == 'R') {
//				//Go for right side
//				m_robotDrive.ArcadeDrive(AUTO_TURN_SPEED, -AUTO_TURN_SPEED);
//			}

		} else if (autoTimer->Get() < (AUTO_TURN_TIME + AUTO_DRIVE_TIME + AUTO_FINAL_TIME)) {
			m_robotDrive.ArcadeDrive(AUTO_DRIVE_SPEED, 0);
		}
		// eject time
		else {
			m_robotDrive.ArcadeDrive(0, 0);
			if (switchSide == 'L') {
				m_clawDrive.ArcadeDrive(-1, 0);
			}

		}

		/*if (position == 1) {
			// LEFT SIDE AUTO PHASE
			if (autoTimer->Get() < AUTO_DRIVE_TIME) {
				m_robotDrive.ArcadeDrive(AUTO_DRIVE_SPEED, 0); //10.72.67.2 rio ip
			} else if (autoTimer->Get() < (AUTO_TURN_TIME + AUTO_DRIVE_TIME)) {
				m_robotDrive.ArcadeDrive(AUTO_TURN_SPEED, -AUTO_TURN_SPEED);
			} else if (autoTimer->Get() < (AUTO_TURN_TIME + AUTO_DRIVE_TIME + AUTO_FINAL_TIME)) {
				m_robotDrive.ArcadeDrive(AUTO_DRIVE_SPEED, 0);
			} else {
				m_robotDrive.ArcadeDrive(0, 0);
				if (switchSide == 'L') {
					m_clawDrive.ArcadeDrive(-1, 0);
				}
			}
		}
		if (position == 3) {
			// RIGHT SIDE AUTO PHASE
			if (autoTimer->Get() < AUTO_DRIVE_TIME_R) {
				m_robotDrive.ArcadeDrive(AUTO_DRIVE_SPEED, 0); //10.72.67.2 rio ip
			} else if (autoTimer->Get() < (AUTO_TURN_TIME_R + AUTO_DRIVE_TIME_R)) {
				m_robotDrive.ArcadeDrive(AUTO_TURN_SPEED, -AUTO_TURN_SPEED);
			} else if (autoTimer->Get() < (AUTO_TURN_TIME_R + AUTO_DRIVE_TIME_R + AUTO_FINAL_TIME_R)) {
				m_robotDrive.ArcadeDrive(AUTO_DRIVE_SPEED, 0);
			} else {
				m_robotDrive.ArcadeDrive(0, 0);
				if (switchSide == 'R') {
					m_clawDrive.ArcadeDrive(-1, 0);
				}
			}
		} */
	}
};

START_ROBOT_CLASS(Robot)
