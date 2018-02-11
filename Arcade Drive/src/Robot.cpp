/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <Drive/DifferentialDrive.h>
#include <IterativeRobot.h>
#include <Joystick.h>
#include <Spark.h>

#include <iostream>

/**
 * This is a demo program showing the use of the DifferentialDrive class.
 * Runs the motors with arcade steering.
 */
class Robot : public frc::IterativeRobot {

	frc::Spark m_leftMotor{1};
	frc::Spark m_rightMotor{0};
	frc::DifferentialDrive m_robotDrive{m_leftMotor, m_rightMotor};
	frc::Joystick m_stick{0};

	frc::Spark m_leftArmMotor{2};

public:

	void TeleopInit() {
	}

	void TeleopPeriodic() {
		// drive with arcade style
		std::cout << m_stick.GetY() << " " << m_stick.GetX() << std::endl;
		m_robotDrive.ArcadeDrive(m_stick.GetY(), m_stick.GetX());
		m_leftArmMotor.Set(1.0);
	}
};

START_ROBOT_CLASS(Robot)
