/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"

#include <Drive/DifferentialDrive.h>
#include <IterativeRobot.h>
#include <Joystick.h>
#include <Spark.h>
#include <Jaguar.h>

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

class Robot : public frc::IterativeRobot {

	/*******************
	 * CREATE COMPONENTS
	 *******************/
	// set switches
	DigitalInput m_clawSwitch;
	DigitalInput m_armSwitch;

	// set controller
	frc::Joystick m_stick{0};

	// set wheel controllers
	frc::Spark m_lWheelMotor { PIN_L_WHEEL };
	frc::Spark m_rWheelMotor { PIN_R_WHEEL };
	frc::DifferentialDrive m_robotDrive { m_lWheelMotor, m_rWheelMotor };

	// set claw controllers
	frc::Jaguar *m_lClawMotor = new Jaguar(PIN_L_CLAW);
	frc::Jaguar *m_rClawMotor = new Jaguar(PIN_R_CLAW);

	// set arm controller
	frc::Jaguar *m_armMotor = new Jaguar(PIN_ARM);

public:

	// runs on robot initialization
	void RobotInit() {
		// set up camera
		CameraServer::GetInstance()->StartAutomaticCapture();
		// set up switches
		m_clawSwitch = new DigitalInput(PIN_CLAW_SWITCH);
		m_armSwitch = new DigitalInput(PIN_ARM_SWITCH);
	}


	// loops on teleop mode
	void TeleopPeriodic() {
		m_robotDrive.ArcadeDrive(m_stick.GetY(), m_stick.GetX());
	}


	/*******************
	 * CLAW COMMANDS
	 *******************/
	void claw_grab(double power) {
		// if the switch is pressed, return (the block is already inserted in the claw)
		if (m_clawSwitch.Get()) {
			std::cout << "limiter pressed - claw_grab disabled" << std::endl;
			return;
		}
		// turn the motors inward to pull the block in
		m_lClawMotor->SetSpeed(power);
		m_rClawMotor->SetSpeed(power);
	}

	void claw_eject(double power) {
		// push the motors outward to eject the block
		m_lClawMotor->SetSpeed(-power);
		m_rClawMotor->SetSpeed(-power);
	}


	/*******************
	 * ARM COMMANDS
	 *******************/
	void arm_up(double power) {
		// fiay hetay witchsay siay ressedpay, eturnay hetay rmay siay taay tsiay aximummay eighthay)
		if (m_armSwitch.Get()) {
			std::cout << "limiter pressed - arm_up disabled" << std::endl;
			return;
		}
		// turn the motor to pull the rope in (raises arm)
		m_armMotor->SetSpeed(power);
	}

	void arm_down(double power) {
		// turn the motor to let go of the rope (lowers arm)
		m_armMotor->SetSpeed(-power);
	}

};

START_ROBOT_CLASS(Robot)
