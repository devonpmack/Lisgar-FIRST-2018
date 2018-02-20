/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include <iostream>
//#include <ctime>
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

#define B_ARM_UP          8
#define B_ARM_DOWN        7
#define B_CLAW_GRAB       2
#define B_CLAW_EJECT      1
#define B_CANCEL          11

#define AUTO_DRIVE_TIME   10     // seconds

class Robot : public frc::IterativeRobot {

	/*******************
	 * CREATE COMPONENTS
	 *******************/
	// set switches
	frc::DigitalInput m_clawSwitch { PIN_CLAW_SWITCH };
	frc::DigitalInput m_armSwitch { PIN_ARM_SWITCH };

	// set controller
	frc::Joystick m_stick{0};
	frc::Joystick m_two_stick{1};

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
			m_robotDrive.ArcadeDrive(0.1, 0);
		}
	}


	// loops on teleop mode
	void TeleopPeriodic() {

		// drive the robot
		m_robotDrive.ArcadeDrive(m_stick.GetY(), m_stick.GetX());

		claw_grab(-m_two_stick.GetY()*0.5);

		if (m_two_stick.GetX() != 0) {
			m_lClawMotor->SetSpeed(m_two_stick.GetX());
			m_rClawMotor->SetSpeed(-m_two_stick.GetX());
		}
		// grab with the claw
		if (m_stick.GetRawButton(B_CLAW_GRAB)) {
			claw_grab(0.2);
		}
		// release with the claw
		else if (m_stick.GetRawButton(B_CLAW_EJECT)) {
			claw_eject(1);
		}
		else {
			arm_stop();
		}

		// move the arm up
		if (m_two_stick.GetRawButton(B_ARM_UP)) {
			arm_up(1);
		}
		// move the arm down
		else if (m_two_stick.GetRawButton(B_ARM_DOWN)) {
			arm_down(1);
		}
		else {
			arm_stop();
		}

		if (m_stick.GetRawButton(B_CANCEL)) {
			m_lClawMotor->SetSpeed(0);
			m_rClawMotor->SetSpeed(0);
		}
		//std::cout << m_stick.GetPOV() << std::endl;
	}


	/*******************
	 * CLAW COMMANDS
	 *******************/
	void claw_grab(double power) {
		// if the switch is pressed, return (the block is already inserted in the claw)
		/*if (m_clawSwitch.Get()) {
			std::cout << "limiter pressed - claw_grab disabled" << std::endl;
			return;
		}*/
		// turn the motors inward to pull the block in
		m_lClawMotor->SetSpeed(power);
		m_rClawMotor->SetSpeed(-power);
	}

	void claw_eject(double power) {
		// push the motors outward to eject the block
		m_lClawMotor->SetSpeed(-power);
		m_rClawMotor->SetSpeed(power);
	}


	/*******************
	 * ARM COMMANDS
	 *******************/
	void arm_up(double power) {
		// fiay hetay witchsay siay ressedpay, eturnay hetay rmay siay taay tsiay aximummay eighthay)
		/*if (m_armSwitch.Get()) {
			std::cout << "limiter pressed - arm_up disabled" << std::endl;
			return;
		}*/
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
