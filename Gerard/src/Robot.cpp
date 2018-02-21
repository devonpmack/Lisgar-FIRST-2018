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
#define PIN_CLAW_SWITCH   5
#define PIN_ARM           4
#define PIN_ARM_SWITCH    6
#define PIN_V_CAM         7
#define PIN_H_CAM         8

//#define B_ARM_UP        8
//#define B_ARM_DOWN      7
#define B_CLAW_GRAB       2
#define B_CLAW_EJECT      1
#define B_CLAW_TURNL      3
#define B_CLAW_TURNR      4

#define AUTO_DRIVE_TIME   10     // seconds
#define AUTO_SPEED        0.1

class Robot : public frc::IterativeRobot {

	/*******************
	 * CREATE COMPONENTS
	 *******************/
	// set switches
	frc::DigitalInput m_clawSwitch { PIN_CLAW_SWITCH };
	frc::DigitalInput m_armSwitch { PIN_ARM_SWITCH };

	// set controller
	frc::Joystick m_stick1{0};
	frc::Joystick m_stick2{1};

	// set wheel controllers
	frc::Spark m_lWheelMotor { PIN_L_WHEEL };
	frc::Spark m_rWheelMotor { PIN_R_WHEEL };
	frc::DifferentialDrive m_robotDrive { m_lWheelMotor, m_rWheelMotor };

	// set claw controllers
	frc::Jaguar *m_lClawMotor = new Jaguar(PIN_L_CLAW);
	frc::Jaguar *m_rClawMotor = new Jaguar(PIN_R_CLAW);

	// set arm controller
	frc::Jaguar *m_armMotor = new Jaguar(PIN_ARM);

	frc::Timer *autoTimer = new Timer();

public:

	Robot() {}

	// runs on robot initialization
	void RobotInit() {
		// set up camera
		CameraServer::GetInstance()->StartAutomaticCapture();
	}

	void AutonomousInit() {
		autoTimer->Start();
		autoTimer->Reset();
	}

	void AutonomousPeriodic() {
		while (autoTimer->Get() < AUTO_DRIVE_TIME * 1000) {            // milliseconds
			m_robotDrive.ArcadeDrive(AUTO_SPEED, 0);
		}
	}


	// loops on teleop mode
	void TeleopPeriodic() {

		// drive the robot with the first joystick
		m_robotDrive.ArcadeDrive(m_stick1.GetY(), m_stick1.GetX());

		// move the arm with the second joystick
		arm_up(-m_stick2.GetY() * 0.5);

		// grab the claw with the joystick trigger
		if (m_stick2.GetRawButton(B_CLAW_GRAB)) {
			claw_grab(0.2);
		}
		// eject the claw with the joystick thumb button
		else if (m_stick2.GetRawButton(B_CLAW_EJECT)) {
			claw_eject(1);
		}
		// turn the block left with the joystick left button (3)
		else if (m_stick2.GetRawButton(B_CLAW_TURNL)) {
			claw_turnBlock(0.5);
		}
		// turn the block right with the joystick right button (4)
		else if (m_stick2.GetRawButton(B_CLAW_TURNR)) {
			claw_turnBlock(-0.5);
		}
		// stop the claw if the claw isn't moving
		else {
			claw_stop();
		}

		/*if (m_stick2.GetX() != 0) {
			m_lClawMotor->SetSpeed(m_stick2.GetX());
			m_rClawMotor->SetSpeed(-m_stick2.GetX());
		}

		// grab with the claw
		if (m_stick1.GetRawButton(B_CLAW_GRAB)) {
			claw_grab(0.2);
		}
		// release with the claw
		else if (m_stick1.GetRawButton(B_CLAW_EJECT)) {
			claw_eject(1);
		}
		else {
			claw_stop();
		}

		// move the arm up
		if (m_stick2.GetRawButton(B_ARM_UP)) {
			arm_up(1);
		}
		// move the arm down
		else if (m_stick2.GetRawButton(B_ARM_DOWN)) {
			arm_down(1);
		}
		else {
			arm_stop();
		}*/

		/*if (m_stick1.GetRawButton(B_CANCEL)) {
			claw_stop();
		}*/
		//std::cout << m_stick.GetPOV() << std::endl;
	}


	/*******************
	 * CLAW COMMANDS
	 *******************/
	void claw_grab(double power) {
		// turn the motors inward to pull the block in
		m_lClawMotor->SetSpeed(power);
		m_rClawMotor->SetSpeed(-power);
	}

	void claw_eject(double power) {
		// push the motors outward to eject the block
		m_lClawMotor->SetSpeed(-power);
		m_rClawMotor->SetSpeed(power);
	}

	void claw_turnBlock(double power) {
		// turn the motors in the same direction (theoretically rotates block)
		m_lClawMotor->SetSpeed(power);
		m_rClawMotor->SetSpeed(power);
	}

	void claw_stop() {
		m_lClawMotor->SetSpeed(0);
		m_rClawMotor->SetSpeed(0);
	}


	/*******************
	 * ARM COMMANDS
	 *******************/
	void arm_up(double power) {
		// turn the motor to pull the rope in (raises arm)
		m_armMotor->SetSpeed(-power);
	}

	void arm_down(double power) {
		// turn the motor to let go of the rope (lowers arm)
		m_armMotor->SetSpeed(power);
	}

	void arm_stop() {
		m_armMotor->SetSpeed(0);
	}

};

START_ROBOT_CLASS(Robot)
