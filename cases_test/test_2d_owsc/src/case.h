/* -------------------------------------------------------------------------*
*								SPHinXsys									*
* --------------------------------------------------------------------------*
* SPHinXsys (pronunciation: s'finksis) is an acronym from Smoothed Particle	*
* Hydrodynamics for industrial compleX systems. It provides C++ APIs for	*
* physical accurate simulation and aims to model coupled industrial dynamic *
* systems including fluid, solid, multi-body dynamics and beyond with SPH	*
* (smoothed particle hydrodynamics), a meshless computational method using	*
* particle discretization.													*
*																			*
* SPHinXsys is partially funded by German Research Foundation				*
* (Deutsche Forschungsgemeinschaft) DFG HU1527/6-1, HU1527/10-1				*
* and HU1527/12-1.															*
*                                                                           *
* Portions copyright (c) 2017-2020 Technical University of Munich and		*
* the authors' affiliations.												*
*                                                                           *
* Licensed under the Apache License, Version 2.0 (the "License"); you may   *
* not use this file except in compliance with the License. You may obtain a *
* copy of the License at http://www.apache.org/licenses/LICENSE-2.0.        *
*                                                                           *
* --------------------------------------------------------------------------*/
/**
* @file 	case.h
* @brief 	This is the case file for the test of Oscillating Wave Surge Converter (OWSC).
* @author   Chi Zhang and Xiangyu Hu
* @version 0.1
* @note  	Observer, moving with mobile solid body, can find template in this case.
*			-- Chi ZHANG
*/
#pragma once

#include "sphinxsys.h"
using namespace SPH;
#define PI 3.1415926
//
Real total_physical_time = 12.0;
//for geometry
Real DL = 18.42; //tank length
Real DH = 1.0; //tank height
Real DL_Extra = 1.0; // for wave maker
Real Water_H = 0.691; /**< Water height. */

Real Flap_width = 0.12;
Real Flap_x     = 7.92;
Real Flap_H     = 0.48;

Real Base_bottom_position = 0.155;
Real Base_height = 0.1;
Real particle_spacing_ref = Flap_width / 4.0; //particle spacing
Real BW = particle_spacing_ref * 4.0; //boundary width

//the offset that the rubber flap shifted above the tank
//Real flap_off = Flap_x - 0.5 * Flap_width + DL_Extra + BW;
//Real off_set = particle_spacing_ref + floor(flap_off / particle_spacing_ref) * particle_spacing_ref - flap_off;
Vec2d offset = Vec2d(0.0);

//define Dam domain
Vec2d Water_lb(0.0, 0.0); //left bottom
Vec2d Water_lt(0.0, Water_H); //left top
Vec2d Water_rt(DL, Water_H); //right top
Vec2d Water_rb(DL, 0.356); //right bottom
Vec2d Water_slope_1(DL - 6.2, 0.356);
Vec2d Water_slope_2(DL - 6.2 - 3.7, 0.155);
Vec2d Water_slope_3(DL - 6.2 - 3.7 - 2.4, 0.155);
Vec2d Water_slope_4(DL - 6.2 - 3.7 - 2.4 - 1.3, 0.0);

//define constrain region of flap
Vec2d Base_lb(Flap_x - 0.5 * Flap_width, Base_bottom_position); //left bottom
Vec2d Base_lt(Flap_x - 0.5 * Flap_width, Base_bottom_position + Base_height); //left top
Vec2d Base_rt(Flap_x + 0.5 * Flap_width, Base_bottom_position + Base_height); //right top
Vec2d Base_rb(Flap_x + 0.5 * Flap_width, Base_bottom_position ); //right bottom

//define flap
Vec2d Flap_lb(Flap_x - 0.5 * Flap_width, Base_bottom_position + Base_height + 0.5 * Flap_width); //left bottom
Vec2d Flap_lt(Flap_x - 0.5 * Flap_width, Base_bottom_position + Base_height + 0.5 * Flap_width + Flap_H); //left top
Vec2d Flap_rt(Flap_x + 0.5 * Flap_width, Base_bottom_position + Base_height + 0.5 * Flap_width + Flap_H); //right top
Vec2d Flap_rb(Flap_x + 0.5 * Flap_width, Base_bottom_position + Base_height + 0.5 * Flap_width); //right bottom

//gravity va
Real gravity_g = 9.81;

//for material properties of the fluid
Real rho0_f = 1000.0;
Real U_f    = 2.0*sqrt(0.79 * gravity_g);
Real c_f    = 10.0*U_f;
Real mu_f   = 1.0e-6;
//for material properties of the solid
Real flap_mass = 33.04;
Real flap_vol  = 0.0579;
Real rho0_s = flap_mass / flap_vol;
Real poisson = 0.33;
Real Youngs_modulus = 7.8e6;
/**
* @brief define geometry and initial conditions of SPH bodies
*/
/**
* @brief create a water block shape
*/
std::vector<Point> CreatWaterBlockShape()
{
	std::vector<Point> pnts;
	pnts.push_back(Water_lb);
	pnts.push_back(Water_lt);
	pnts.push_back(Water_rt);
	pnts.push_back(Water_rb);
	pnts.push_back(Water_slope_1);
	pnts.push_back(Water_slope_2);
	pnts.push_back(Water_slope_3);
	pnts.push_back(Water_slope_4);
	pnts.push_back(Water_lb);

	return pnts;
}
/**
* @brief create a damping zone
*/
std::vector<Point> CreatDampingBufferShape()
{
	std::vector<Point> pnts;
	pnts.push_back(Point(DL - 5.0,  0.356 - BW));
    pnts.push_back(Point(DL - 5.0,  DH));
    pnts.push_back(Point(DL + BW,   DH));
    pnts.push_back(Point(DL + BW,   0.356 - BW));
    pnts.push_back(Point(DL - 5.0,  0.356 - BW));

	return pnts;
}
/**
* @brief create flap base shape
*/
std::vector<Point> CreatFlapConstrainShape()
{
	std::vector<Point> pnts2;
	pnts2.push_back(Base_lb);
	pnts2.push_back(Base_lt);
	pnts2.push_back(Base_rt);
	pnts2.push_back(Base_rb);
	pnts2.push_back(Base_lb);

	return pnts2;
}
/**
* @brief create flap base shape
*/
std::vector<Point> CreatFlapShape()
{
	std::vector<Point> pnts3;
	pnts3.push_back(Flap_lb);
	pnts3.push_back(Flap_lt);
	pnts3.push_back(Flap_rt);
	pnts3.push_back(Flap_rb);
    for(int i = 1; i <= 10; i++)
    {
        Real angle =  Real(i) * PI / (11.0);
        Real x = Flap_rb[0] - 0.5 * Flap_width * (1.0 - cos(angle));
        Real y = Flap_rb[1] - 0.5 * Flap_width * sin(angle) - 0.5 * particle_spacing_ref;
        pnts3.push_back(Point(x,y));
    }
	pnts3.push_back(Flap_lb);

	return pnts3;
}
/**
* @brief create outer wall shape
*/
std::vector<Point> CreatOuterWallShape()
{
	std::vector<Point> pnts1;
	pnts1.push_back(Point(-DL_Extra - BW, -BW));
	pnts1.push_back(Point(-DL_Extra - BW, DH + BW));
	pnts1.push_back(Point(DL + BW, DH + BW));
	pnts1.push_back(Point(DL + BW, 0.35 - BW));
	pnts1.push_back(Water_slope_1 + Vec2d(0.0, -BW));
	pnts1.push_back(Water_slope_2 + Vec2d(0.0, -BW));
	pnts1.push_back(Water_slope_3 + Vec2d(0.0, -BW));
	pnts1.push_back(Water_slope_4 + Vec2d(0.0, -BW));
	pnts1.push_back(Point(-DL_Extra - BW, -BW));

	return pnts1;
}
/**
* @brief create inner wall shape 01
*/
std::vector<Point> CreatInnerWallShape01()
{
	std::vector<Point> pnts2;
	pnts2.push_back(Water_lb);
	pnts2.push_back(Point(0.0, DH + BW));
	pnts2.push_back(Point(DL, DH + BW));
	pnts2.push_back(Water_rb);
	pnts2.push_back(Water_slope_1);
	pnts2.push_back(Water_slope_2);
	pnts2.push_back(Base_rb);
    pnts2.push_back(Base_rt);
    pnts2.push_back(Base_lt);
    pnts2.push_back(Base_lb);
	pnts2.push_back(Water_slope_3);
	pnts2.push_back(Water_slope_4);
	pnts2.push_back(Water_lb);

	return pnts2;
}
/**
* @brief create inner wall shape 02
*/
std::vector<Point> CreatInnerWallShape02()
{
	std::vector<Point> pnts3;
	pnts3.push_back(Point(-DL_Extra, 0.0));
	pnts3.push_back(Point(-DL_Extra, DH + BW));
	pnts3.push_back(Point(-BW, DH + BW));
	pnts3.push_back(Point(-BW, 0.0));
	pnts3.push_back(Point(-DL_Extra, 0.0));

	return pnts3;
}
/**
* @brief create wave maker shape
*/
std::vector<Point> CreatWaveMakerShape()
{
	std::vector<Point> wave_make_shape;
	wave_make_shape.push_back(Point(-BW, 0.0));
	wave_make_shape.push_back(Point(-BW, DH + BW));
	wave_make_shape.push_back(Point(0.0, DH + BW));
	wave_make_shape.push_back(Point(0.0, 0.0));
	wave_make_shape.push_back(Point(-BW, 0.0));

	return wave_make_shape;
}
//define the fluid body
class WaterBlock : public FluidBody
{
	public:
		WaterBlock(SPHSystem &system, string body_name,
			int refinement_level)
			: FluidBody(system, body_name, refinement_level)
		{
			std::vector<Point> water_block_shape = CreatWaterBlockShape();
			std::vector<Point> flap_shape = CreatFlapShape();
            std::vector<Point> flap_base_shape = CreatFlapConstrainShape();
			body_shape_ = new ComplexShape(body_name);
			body_shape_->addAPolygon(water_block_shape, ShapeBooleanOps::add);
			body_shape_->addAPolygon(flap_shape, ShapeBooleanOps::sub);
		    body_shape_->addAPolygon(flap_base_shape, ShapeBooleanOps::sub);
		}
};
/**
 * @brief 	Case dependent material properties definition.
 */
class WaterMaterial : public WeaklyCompressibleFluid
{
public:
	WaterMaterial() : WeaklyCompressibleFluid()
	{
		rho_0_ = rho0_f;
		c_0_ = c_f;
        mu_ = mu_f;

		assignDerivedMaterialParameters();
	}
};
/** Damping buffer */
class DampingBuffer : public BodyPartByCell
{
public:
	DampingBuffer(FluidBody* fluid_body, string constrained_region_name)
		: BodyPartByCell(fluid_body, constrained_region_name)
	{
		/** Geometry definition. */
		body_part_shape_ = new ComplexShape(constrained_region_name);
		std::vector<Point> buffer_shape = CreatDampingBufferShape();
		body_part_shape_->addAPolygon(buffer_shape, ShapeBooleanOps::add);
		/** tag the constrained particle. */
		tagBodyPart();
	}
};
/** define the static solid wall boundary. */
class WallBoundary : public SolidBody
{
public:
	WallBoundary(SPHSystem &system, string body_name, 
		int refinement_level)
		: SolidBody(system, body_name, refinement_level)
	{
		body_shape_ = new ComplexShape(body_name);
		std::vector<Point> outer_wall_shape   = CreatOuterWallShape();
		std::vector<Point> inner_wall_shape_1 = CreatInnerWallShape01();
		std::vector<Point> inner_wall_shape_2 = CreatInnerWallShape02();
		body_shape_->addAPolygon(outer_wall_shape, ShapeBooleanOps::add);
        std::vector<Point> flap_base_shape = CreatFlapConstrainShape();
		body_shape_->addAPolygon(flap_base_shape, ShapeBooleanOps::add);
		body_shape_->addAPolygon(inner_wall_shape_1, ShapeBooleanOps::sub);
		body_shape_->addAPolygon(inner_wall_shape_2, ShapeBooleanOps::sub);
	}
};

//define the elastic flap
class Flap : public SolidBody
{
public:
	Flap(SPHSystem &system, string body_name, int refinement_level)
		: SolidBody(system, body_name, refinement_level)
	{
		body_shape_ = new ComplexShape(body_name);
		std::vector<Point> flap_shape = CreatFlapShape();
		body_shape_->addAPolygon(flap_shape, ShapeBooleanOps::add);
	}
};
/**
* @brief create fish head for constraint
*/
class FlapSystemForSimbody : public SolidBodyPartForSimbody
{
	void tagBodyPart() override
	{
		BodyPartByParticle::tagBodyPart();
		//Vecd mass_center = Vecd(7.92, 0.355); // 0.3355
		//initial_mass_center_ = SimTK::Vec3(mass_center[0], mass_center[1], 0.0);
		/** UnitInertia_ (const RealP &xx, const RealP &yy, const RealP &zz)
		 * 	Create a principal unit inertia matrix (only non-zero on diagonal). 
		 */
		Real Iz = 1.84 / 33.04;
		body_part_mass_properties_
			= new SimTK::MassProperties(33.04, SimTK::Vec3(0.0), SimTK::UnitInertia(0.0, 0.0, Iz));
	}
public:
	FlapSystemForSimbody(SolidBody* solid_body,
		string constrained_region_name, Real solid_body_density)
		: SolidBodyPartForSimbody(solid_body, constrained_region_name)
	{
		body_part_shape_ = new ComplexShape(constrained_region_name);
		std::vector<Point> flap_shape = CreatFlapShape();
		body_part_shape_->addAPolygon(flap_shape, ShapeBooleanOps::add);
		/** tag the constrained particle. */
		tagBodyPart();
	}
};
/**
 * @brief Define Flap material.
 */
class FlapMaterial : public LinearElasticSolid
{
public:
	FlapMaterial() : LinearElasticSolid()
	{
		rho_0_ = rho0_s;
		E_0_ = Youngs_modulus;
		nu_ = poisson;

		assignDerivedMaterialParameters();
	}
};
/**
* @brief constrain the beam base
*/
class WaveMaker : public BodyPartByParticle
{
public:
	WaveMaker(SolidBody *solid_body, string constrained_region_name)
		: BodyPartByParticle(solid_body, constrained_region_name)
	{
		body_part_shape_ = new ComplexShape(constrained_region_name);
		std::vector<Point> wave_maker_shape = CreatWaveMakerShape();
		body_part_shape_->addAPolygon(wave_maker_shape, ShapeBooleanOps::add);
		/** tag the constrained particle. */
		tagBodyPart();
	}
};
/**
* @brief making the wave
*/
class WaveMaking : public solid_dynamics::ConstrainSolidBodyRegion
{
	Real model_scale_;
	Real gravity_;
	Real water_depth_;
	Real wave_height_;
	Real wave_period_;
	Real wave_freq_;
	Real wave_stroke_;
	Real time_;

	virtual Point GetDisplacement(Point& pos_0, Point& pos_n) override
	{
		Point displacement(0);
		displacement[0] = 0.5 * wave_stroke_ * sin(wave_freq_ * time_);
		return pos_0 + displacement;
	}

	virtual Vec2d GetVelocity(Point& pos_0, Point& pos_n, Vec2d& vel_n) override 
	{
		Vec2d velocity(0);
		velocity[0] = 0.5 * wave_stroke_ * wave_freq_ * cos(wave_freq_ * time_);
		return velocity;
	}

	virtual Vec2d GetAcceleration(Point& pos_0, Point& pos_n, Vec2d& dvel_dt) override
	{
		Vec2d acceleration(0);
		acceleration[0] = - 0.5 * wave_stroke_ * wave_freq_ * wave_freq_ * sin(wave_freq_ * time_);
		return acceleration;
	}

	virtual void setupDynamics(Real dt = 0.0) override
	{
		body_->setNewlyUpdated();
		time_ = GlobalStaticVariables::physical_time_;
	}

	void computeWaveStrokeAndFrequency( )
	{
		Real scaled_wave_height = wave_height_ / model_scale_;
		Real scaled_wave_period = wave_period_ / sqrt(model_scale_);
		Real scaled_wave_freq = 2.0 * PI / scaled_wave_period;
		Real scaled_wave_amp  = 0.5 * scaled_wave_height;

		int iterator = 20; 
		Real Tol = 1.0e-6;
		Real scaled_wave_number = 1.0;
		for(int i = 1; i < iterator; i++)
		{
			Real term1 = tanh(scaled_wave_number * water_depth_);
			Real term2 = scaled_wave_freq * scaled_wave_freq / gravity_;
			Real term3 = scaled_wave_number * term1 - term2;
			Real term4 = term1 + scaled_wave_number * water_depth_ * (1.0 - term1 * term1);
			Real wave_number_old = scaled_wave_number;
			scaled_wave_number = wave_number_old - term3 / term4;
			Real error = abs(scaled_wave_number - wave_number_old) / abs(scaled_wave_number);
			if(error <= Tol) 
				break;
		}

		Real term_1 = gravity_ / scaled_wave_freq / scaled_wave_freq;
		Real term_2 = 2.0 * scaled_wave_number * water_depth_;
		Real term_3 = scaled_wave_number * water_depth_;
		Real scaled_wave_stroke = 0.5 * scaled_wave_amp * scaled_wave_number * term_1*                  
			(term_2 + sinh(term_2))/ (cosh(term_3) * sinh(term_3));

		wave_stroke_ = scaled_wave_stroke;
		wave_freq_ = scaled_wave_freq;
		std::cout<< "Wave stroke: " << wave_stroke_ << " Wave frequency: " << wave_freq_ << std::endl;
	}
public:
	WaveMaking(SolidBody *solid_body, BodyPartByParticle*constrained_region)
		: ConstrainSolidBodyRegion(solid_body, constrained_region), time_(0.0)
	{
		model_scale_ = 25.0;
		wave_height_ = 5.0;
		wave_period_ = 10.0;
		gravity_ 	 = gravity_g;
		water_depth_ = Water_H;
		computeWaveStrokeAndFrequency();
	}
};
/**
* @brief create a damping zone
*/
Real h = 1.3 * particle_spacing_ref;
std::vector<Point> CreatWaveProbeShape4()
{
	std::vector<Point> pnts;
	pnts.push_back(Point(3.99 - h,  0.0));
    pnts.push_back(Point(3.99 - h,  1.0));
    pnts.push_back(Point(3.99 + h, 1.0));
    pnts.push_back(Point(3.99 + h,  0.0));
    pnts.push_back(Point(3.99 - h,  0.0));

	return pnts;
}
std::vector<Point> CreatWaveProbeShape5()
{
	std::vector<Point> pnts;
	pnts.push_back(Point(7.02 - h,  0.155));
    pnts.push_back(Point(7.02 - h,  1.0));
    pnts.push_back(Point(7.02 + h,  1.0));
    pnts.push_back(Point(7.02 + h,  0.155));
    pnts.push_back(Point(7.02 - h,  0.155));

	return pnts;
}

std::vector<Point> CreatWaveProbeShape12()
{
	std::vector<Point> pnts;
	pnts.push_back(Point(8.82 - h,  0.155));
    pnts.push_back(Point(8.82 - h,  1.0));
    pnts.push_back(Point(8.82 + h,  1.0));
    pnts.push_back(Point(8.82 + h,  0.155));
    pnts.push_back(Point(8.82 - h,  0.155));

	return pnts;
}
/** WaveProbe buffer */
class WaveProbeBufferNo4 : public BodyPartByCell
{
public:
	WaveProbeBufferNo4(FluidBody* fluid_body, string constrained_region_name)
		: BodyPartByCell(fluid_body, constrained_region_name)
	{
		/** Geometry definition. */
		body_part_shape_ = new ComplexShape(constrained_region_name);
		std::vector<Point> buffer_shape = CreatWaveProbeShape4();
		body_part_shape_->addAPolygon(buffer_shape, ShapeBooleanOps::add);
		/** tag the constrained particle. */
		tagBodyPart();
	}
};
/** WaveProbe buffer */
class WaveProbeBufferNo5 : public BodyPartByCell
{
public:
	WaveProbeBufferNo5(FluidBody* fluid_body, string constrained_region_name)
		: BodyPartByCell(fluid_body, constrained_region_name)
	{
		/** Geometry definition. */
		body_part_shape_ = new ComplexShape(constrained_region_name);
		std::vector<Point> buffer_shape = CreatWaveProbeShape5();
		body_part_shape_->addAPolygon(buffer_shape, ShapeBooleanOps::add);
		/** tag the constrained particle. */
		tagBodyPart();
	}
};
/** WaveProbe buffer */
class WaveProbeBufferNo12 : public BodyPartByCell
{
public:
	WaveProbeBufferNo12(FluidBody* fluid_body, string constrained_region_name)
		: BodyPartByCell(fluid_body, constrained_region_name)
	{
		/** Geometry definition. */
		body_part_shape_ = new ComplexShape(constrained_region_name);
		std::vector<Point> buffer_shape = CreatWaveProbeShape12();
		body_part_shape_->addAPolygon(buffer_shape, ShapeBooleanOps::add);
		/** tag the constrained particle. */
		tagBodyPart();
	}
};
/** Flap observer body */
class FlapObserver : public FictitiousBody
{
public:
	FlapObserver(SPHSystem& system, string body_name, int refinement_level)
		: FictitiousBody(system, body_name, refinement_level, 1.3)
	{
		/** the measuring particle with zero volume */
		body_input_points_volumes_.push_back(make_pair(Point(7.862, 0.645), 0.0));
		body_input_points_volumes_.push_back(make_pair(Point(7.862, 0.741), 0.0));
		body_input_points_volumes_.push_back(make_pair(Point(7.862, 0.391), 0.0));
		body_input_points_volumes_.push_back(make_pair(Point(7.862, 0.574), 0.0));
		body_input_points_volumes_.push_back(make_pair(Point(7.862, 0.716), 0.0));
		body_input_points_volumes_.push_back(make_pair(Point(7.862, 0.452), 0.0));
	}
};
