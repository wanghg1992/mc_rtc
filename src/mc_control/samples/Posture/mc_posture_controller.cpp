/*
 * Copyright 2015-2019 CNRS-UM LIRMM, CNRS-AIST JRL
 */

#include "mc_posture_controller.h"

#include <mc_rtc/logging.h>

/* Note all service calls except for controller switches are implemented in mc_global_controller_services.cpp */

namespace mc_control
{

/* Common stuff */
MCPostureController::MCPostureController(std::shared_ptr<mc_rbdyn::RobotModule> robot_module,
                                         double dt,
                                         Backend backend)
: MCController(robot_module, dt, backend)
{
  qpsolver->addConstraintSet(contactConstraint);
  qpsolver->addConstraintSet(kinematicsConstraint);
  qpsolver->addConstraintSet(selfCollisionConstraint);
  qpsolver->addConstraintSet(*compoundJointConstraint);
  qpsolver->addTask(postureTask.get());
  qpsolver->setContacts({});

  postureTask->stiffness(1.0);
  mc_rtc::log::success("MCPostureController init done");
}

bool MCPostureController::run()
{
  return mc_control::MCController::run();
}

} // namespace mc_control

MULTI_CONTROLLERS_CONSTRUCTOR("Posture",
                              mc_control::MCPostureController(rm, dt, mc_control::MCController::Backend::Tasks),
                              "Posture_TVM",
                              mc_control::MCPostureController(rm, dt, mc_control::MCController::Backend::TVM))
