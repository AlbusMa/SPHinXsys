/**
 * @file muscle_activation.cpp
 * @brief This is the first example of electro activation of myocardium
 * @author Chi Zhang and Xiangyu Hu
 * @version 0.1.0
 */
#include "sphinxsys.h"
/** Name space. */
using namespace SPH;
/** Geometry parameters. */
Real PL = 1.0; 		/**< Length. */
Real PH = 1.0; 		/**< Thickness for thick plate. */
Real PW = 1.0;		/**< Width. */				
/**< Initial particle spacing. */
Real particle_spacing_ref = PH / 25.0;
Real SL = 4.0 * particle_spacing_ref; 		/**< Extension for holder. */
/**< SimTK geometric modeling resolution. */
int resolution(20);
/** For material properties of the solid. */
Real rho_0 = 1.0; 
Real a_0[4] = {0.059, 0.0, 0.0, 0.0};
Real b_0[4] = {8.023, 0.0, 0.0, 0.0};
Vec3d fiber_direction(1.0, 0.0, 0.0);
Vec3d sheet_direction(0.0, 1.0, 0.0);
Real reference_voltage = 30.0;
Real linear_active_stress_factor = - 0.5;
/** reference stress to achieve weakly compressible condition */
Real bulk_modulus = 30.0 * reference_voltage * fabs(linear_active_stress_factor);

/** Define the geometry. */
TriangleMeshShape* CreateMyocardium()
{
	Vecd halfsize_myocardium(0.5 * (PL + SL), 0.5 * PH, 0.5 * PW);
	Vecd translation_myocardium(0.5 * (PL - SL), 0.5 * PH, 0.5*PW);
	TriangleMeshShape* geometry_myocardium = new TriangleMeshShape(halfsize_myocardium, resolution,
		translation_myocardium);

	return geometry_myocardium;
}
/** Define the holder geometry. */
TriangleMeshShape* CreateHolder()
{
	Vecd halfsize_shape(0.5 * SL, 0.5 * PH, 0.5 * PW);
	Vecd translation_shape(-0.5 * SL, 0.5 * PH, 0.5 * PW);
	TriangleMeshShape* geometry = new TriangleMeshShape(halfsize_shape, resolution,
		translation_shape);

	return geometry;
}
/** Define the myocardium body. */
class Myocardium : public SolidBody
{
public:
	Myocardium(SPHSystem &system, string body_name, int refinement_level)
		: SolidBody(system, body_name, refinement_level)
	{
		body_shape_ = new ComplexShape(body_name);
		body_shape_->addTriangleMeshShape(CreateMyocardium(), ShapeBooleanOps::add);
	}
};
/**
* @brief define the Holder base which will be constrained.
* NOTE: this class can only be instanced after body particles
* have been generated
*/
class Holder : public BodyPartByParticle
{
public:
	Holder(SolidBody *solid_body, string constrained_region_name)
		: BodyPartByParticle(solid_body, constrained_region_name)
	{
		body_part_shape_ = new ComplexShape(constrained_region_name);
		body_part_shape_->addTriangleMeshShape(CreateHolder(), ShapeBooleanOps::add);

		tagBodyPart();
	}
};
 /**
 * Assign case dependent muscle activation histroy  
 */
class MyocardiumActivation
	: public active_muscle_dynamics::MuscleActivation
{
public:
	MyocardiumActivation(SolidBody *myocardium)
		: active_muscle_dynamics::MuscleActivation(myocardium) {};
protected:
	void Update(size_t index_i, Real dt) override 
	{
		Real voltage = pos_0_[index_i][0] <= 0 ? 0.0 : reference_voltage * pos_0_[index_i][0] / PL;
		active_contraction_stress_[index_i] += GlobalStaticVariables::physical_time_ <= 1.0
			? linear_active_stress_factor * voltage * dt : 0.0;
	};
};
class ConstrainHolder
	: public solid_dynamics::ConstrainSolidBodyRegion
{
public:
	ConstrainHolder(SolidBody* body, BodyPartByParticle* body_part, int axis_id)
		: solid_dynamics::ConstrainSolidBodyRegion(body, body_part),
		axis_id_(axis_id) {};
protected:
	int axis_id_;
	virtual Point GetDisplacement(Point& pos_0, Point& pos_n) {
		Point pos_temp = pos_n;
		pos_temp[axis_id_] = pos_0[axis_id_];
		return pos_temp; 
	};
	virtual Vecd GetVelocity(Point& pos_0, Point& pos_n, Vecd& vel_n) {
		Vecd vel_temp = vel_n;
		vel_temp[axis_id_] = 0.0;
		return vel_temp; 
	};
	virtual Vecd GetAcceleration(Point& pos_0, Point& pos_n, Vecd& dvel_dt) {
		Vecd dvel_dt_temp = dvel_dt;
		dvel_dt_temp[axis_id_] = 0.0;
		return dvel_dt_temp;
	};
};
 /**
 * Setup material properties of myocardium
 */
 /**
 * Setup material properties of myocardium
 */
class MyocardiumMuscle
	: public Muscle
{
public:
	MyocardiumMuscle() : Muscle()
	{
		rho_0_ = rho_0;
		bulk_modulus_ = bulk_modulus;
		f0_ = fiber_direction;
		s0_ = sheet_direction;
		std::copy(a_0, a_0 + 4, a_0_);
		std::copy(b_0, b_0 + 4, b_0_);

		assignDerivedMaterialParameters();
	}
};
/**
 *  The main program
 */
int main()
{
	/** Setup the system. */
	SPHSystem system(Vecd(-SL, -SL, -SL),
		Vecd(PL + SL, PH + SL, PW + SL), particle_spacing_ref);

	/** Creat a Myocardium body, corresponding material, particles and reaction model. */
	Myocardium *myocardium_muscle_body =
		new Myocardium(system, "MyocardiumMuscleBody", 0);
	ActiveMuscleParticles 	myocardium_muscle_particles(myocardium_muscle_body, new ActiveMuscle(new MyocardiumMuscle()));

	/** topology */
	SPHBodyInnerRelation* myocardium_muscle_body_inner = new SPHBodyInnerRelation(myocardium_muscle_body);

	/** 
	 * This section define all numerical methods will be used in this case.
	 */
	/** Corrected strong configuration. */	
	solid_dynamics::CorrectConfiguration 
		corrected_configuration_in_strong_form(myocardium_muscle_body_inner);
	/** Time step size calculation. */
	solid_dynamics::AcousticTimeStepSize 
		computing_time_step_size(myocardium_muscle_body);
	/** Compute the active contraction stress */
	MyocardiumActivation myocardium_activation(myocardium_muscle_body);
	/** active and passive stress relaxation. */
	solid_dynamics::StressRelaxationFirstHalf
		stress_relaxation_first_half(myocardium_muscle_body_inner);
	solid_dynamics::StressRelaxationSecondHalf
		stress_relaxation_second_half(myocardium_muscle_body_inner);
	/** Constrain region of the inserted body. */
	ConstrainHolder
		constrain_holder(myocardium_muscle_body, new Holder(myocardium_muscle_body, "Holder"), 0);
	/** Output */
	In_Output in_output(system);
	WriteBodyStatesToVtu write_states(in_output, system.real_bodies_);
	/**
	 * From here the time stepping begines.
	 * Set the starting time.
	 */
	GlobalStaticVariables::physical_time_ = 0.0;
	system.initializeSystemCellLinkedLists();
	system.initializeSystemConfigurations();
	corrected_configuration_in_strong_form.parallel_exec();
	write_states.WriteToFile(GlobalStaticVariables::physical_time_);
	/** Setup physical parameters. */
	int ite = 0;
	Real end_time = 1.2;
	Real output_period = end_time / 60.0;		
	Real dt = 0.0; 
	/** Statistics for computing time. */
	tick_count t1 = tick_count::now();
	tick_count::interval_t interval;
	/**
	 * Main loop
	 */
	while (GlobalStaticVariables::physical_time_ < end_time)
	{
		Real integration_time = 0.0;
		while (integration_time < output_period) 
		{
			if (ite % 100 == 0) 
			{
				cout << "N=" << ite << " Time: "
					<< GlobalStaticVariables::physical_time_ << "	dt: "
					<< dt << "\n";
			}
			myocardium_activation.parallel_exec(dt);
			stress_relaxation_first_half.parallel_exec(dt);
			constrain_holder.parallel_exec(dt);
			stress_relaxation_second_half.parallel_exec(dt);

			ite++;
			dt = computing_time_step_size.parallel_exec();
			integration_time += dt;
			GlobalStaticVariables::physical_time_ += dt;
		}

		tick_count t2 = tick_count::now();
		write_states.WriteToFile(GlobalStaticVariables::physical_time_);
		tick_count t3 = tick_count::now();
		interval += t3 - t2;
	}
	tick_count t4 = tick_count::now();

	tick_count::interval_t tt;
	tt = t4 - t1 - interval;
	cout << "Total wall time for computation: " << tt.seconds() << " seconds." << endl;

	return 0;
}
