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

#define B_ARM_UP          2
#define B_ARM_DOWN        1

#define AUTO_DRIVE_TIME   2       // seconds
#define AUTO_DRIVE_SPEED  0.5
#define AUTO_TURN_TIME    2     // seconds
#define AUTO_TURN_SPEED   0.5

class Robot : public frc::IterativeRobot {

	/*******************
	 * CREATE COMPONENTS
	 *******************/

	// set controller
	frc::Joystick m_stick1 { 0 };
	frc::Joystick m_stick2 { 1 };

	// set wheel controllers
	frc::Spark m_lWheelMotor { PIN_L_WHEEL };
	frc::Spark m_rWheelMotor { PIN_R_WHEEL };
	frc::DifferentialDrive m_robotDrive { m_lWheelMotor, m_rWheelMotor };

	// set claw controllers
	frc::Spark m_lClawMotor { PIN_L_CLAW };
	frc::Spark m_rClawMotor { PIN_R_CLAW };
	frc::DifferentialDrive m_clawDrive { m_lClawMotor, m_rClawMotor };

	// set arm controller
	frc::Jaguar *m_armMotor = new Jaguar(PIN_ARM);

	// set the autonomous mode timer
	frc::Timer *autoTimer = new Timer();

	// get the game data
	std::string gameData;
	char switchSide;

public:

	Robot() {}

	// runs on robot initialization
	void RobotInit() {
		// set up camera
		CameraServer::GetInstance()->StartAutomaticCapture();
		// get the game data
		gameData = frc::DriverStation::GetInstance().GetGameSpecificMessage();
		// set the switch side (X if not detected)
		switchSide = (gameData.length() > 0) ? gameData[0] : 'X';
	}

	void AutonomousInit() {
		autoTimer->Start();
		autoTimer->Reset();
	}

	void AutonomousPeriodic() {
		auto_move();
	}


	// loops on teleop mode
	void TeleopPeriodic() {

		// drive the robot with the first joystick
		m_robotDrive.ArcadeDrive(-m_stick1.GetY(), m_stick1.GetX() * 0.6);

		// drive the claws with the second joystick
		//m_clawDrive.ArcadeDrive(m_stick2.GetY(), m_stick2.GetX());
		m_clawDrive.ArcadeDrive(m_stick2.GetZ(), m_stick2.GetX());

		// grab the claw with the joystick trigger
		if (m_stick2.GetRawButton(B_ARM_UP)) {
			m_armMotor->SetSpeed(-0.5);
		}
		// eject the claw with the joystick thumb button
		else if (m_stick2.GetRawButton(B_ARM_DOWN)) {
			m_armMotor->SetSpeed(0.4);
		}
		// stop the claw if the claw isn't moving
		else {
			m_armMotor->SetSpeed(-0.1);
		}

	}


	/*******************
	 * AUTONOMOUS COMMANDS
	 *******************/
	void auto_move() {
		std::cout << autoTimer->Get() << std::endl;
		// drive time
		if (autoTimer->Get() < AUTO_DRIVE_TIME) {
			m_robotDrive.ArcadeDrive(AUTO_DRIVE_SPEED, 0);
		}
		// turn time
		else if (autoTimer->Get() < (AUTO_TURN_TIME + AUTO_DRIVE_TIME)) {
			if (switchSide == 'L') {
				m_robotDrive.ArcadeDrive(-AUTO_TURN_SPEED, AUTO_TURN_SPEED);
			}
			else if (switchSide == 'R') {
				m_robotDrive.ArcadeDrive(AUTO_TURN_SPEED, -AUTO_TURN_SPEED);
			}
		}
		// eject time
		else {
			m_robotDrive.ArcadeDrive(0, 0);
			m_clawDrive.ArcadeDrive(1, 0);
		}
	}

};

START_ROBOT_CLASS(Robot)
