/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include <iostream>

#define LEFT                      0
#define CENTER                    1
#define RIGHT                     2

#define LONG                      0
#define SHORT                     1
#define VERY_SHORT                2

// pins
#define PIN_L_WHEEL               1
#define PIN_R_WHEEL               0
#define PIN_L_CLAW                2
#define PIN_R_CLAW                3
#define PIN_ARM                   4

// buttons and joysticks
#define J_DRIVE                   0
#define J_CLAW                    1
#define B_ARM_UP                  2
#define B_ARM_DOWN                1

// power for turning and driving
#define DRIVE_POWER               0
#define TURN_POWER                0

#define ARM_UP_POWER              0
#define ARM_DOWN_POWER            0

// autonomous timing
#define AUTO_DRIVESHORT_TIME      0
#define AUTO_DRIVELONG_TIME       0
#define AUTO_DRIVEVERYSHORT_TIME  0
#define AUTO_TURN_TIME            0
#define AUTO_EJECT_TIME           0

class Robot : public frc::IterativeRobot {

	// set controller
	frc::Joystick *m_driveStick = new Joystick(J_DRIVE);
	frc::Joystick *m_clawStick = new Joystick(J_CLAW);

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
	int position = 3;
	char switchSide;

	// drive with joystick
	void wheel_drive() {
		m_robotDrive.ArcadeDrive(m_driveStick->GetY() * DRIVE_POWER, m_driveStick.GetX() * TURN_POWER);
	}
	// drive with speed variables
	void wheel_drive(double y, double x) {
		m_robot.ArcadeDrive(y * DRIVE_POWER, x * TURN_POWER);
	}
	// stop wheels
	void wheel_stop() {
		wheel_drive(0, 0);
	}

	// turn claws with joystick
	void claw_drive() {
		m_clawDrive.ArcadeDrive(m_clawStick->GetY(), m_clawStick->GetX());
	}
	// turn claws without joystick
	void claw_drive(double y, double x) {
		m_clawDrive.ArcadeDrive(y, x);
	}
	// stop claws
	void claw_stop() {
		claw_drive(0, 0);
	}

	// autonomous drive (time = time driven, dist = LONG or SHORT)
	void auto_drive(int dist) {
		wheel_drive(AUTO_DRIVE_SPEED, 0);
		// I an so sorry for writing this.
		frc::Wait((dist == LONG) ? AUTO_DRIVELONG_TIME : ((dist == SHORT) ? AUTO_DRIVESHORT_TIME : AUTO_DRIVEVERYSHORT_TIME));
	}

	// autonomous turn (dir = LEFT or RIGHT)
	void auto_turn(int dir) {
		wheel_drive(0, AUTO_TURN_SPEED * ((dir == LEFT) ? -1 : 1));
		frc::Wait(AUTO_TURN_TIME);
	}

	// move arm up (button = requires button press)
	void arm_up(bool button) {
		// return false on failure
		if (button && m_clawStick->GetRawButton(B_ARM_UP))
			return false;
		// return true on success
		m_armMotor->SetSpeed(-ARM_UP_POWER);
		return true;
	}
	// move arm down (button = requires button press)
	void arm_down(bool button) {
		// return false on failure
		if (button && m_clawStick->GetRawButton(B_ARM_DOWN))
			return false;
		// return true on success
		m_armMotor->SetSpeed(ARM_DOWN_POWER);
		return true;
	}

	// stop the arm
	void arm_stop() {
		m_armMotor->SetSpeed(0);
	}

public:

	Robot() {}

	// runs on robot initialization
	void RobotInit() {
		//does nothing? Hopefully you can use this to set the starting position
		position = SmartDashboard::GetNumber("Position", 1);
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
		auto_move();

	}
	void AutonomousPeriodic() {
		SmartDashboard::PutNumber("Seconds Remaining",15-autoTimer->Get());
	}


	// loops on teleop mode
	void TeleopPeriodic() {

		// drive robot
		wheelDrive();

		// mvoe claws
		clawDrive();

		// move arms
		if (!arm_up(true)) {
			if (!arm_down(true)) {
				arm_stop();
			}
		}

		std::cout << m_driveStick->GetRawAxis(3) << std::endl;
	}

	void auto_move() {

		if (position == LEFT || position == RIGHT) {

			// move forward (long)
			auto_drive(LONG);

			// if switch is not on the position side, stop
			if (switchSide != ((position == LEFT) ? 'L' : 'R')) {
				wheel_stop();
				return;
			}

			// turn in the direction opposite of the side of the robot (right if position == LEFT, left if position == RIGHT)
			auto_turn(position);

			// drop block
			claw_drive(1, 0);
			frc::Wait(AUTO_DRIVE_TIME);

			// stop
			claw_stop();
			return;

		}

		if (position == CENTER) {

			// turn in the direction of the side of the switch (right if switchSide == RIGHT, left if switchSide == LEFT)
			wheel_drive(0, AUTO_TURN_SPEED * ((switchSide == LEFT) ? -1 : 1));
			frc::Wait(AUTO_TURN_TIME);

			wheel_drive()
		}
	}
};

START_ROBOT_CLASS(Robot)
