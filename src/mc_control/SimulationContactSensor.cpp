#include <mc_control/SimulationContactSensor.h>

#include <mc_rbdyn/SCHAddon.h>

namespace mc_control
{

SimulationContactPair::SimulationContactPair(const std::shared_ptr<mc_rbdyn::Surface> & robotSurface, const std::shared_ptr<mc_rbdyn::Surface> & envSurface)
: robotSurface(robotSurface), envSurface(envSurface),
  robotSch(mc_rbdyn::surface_to_sch(*(robotSurface.get()), 0.005, 8)),
  envSch(mc_rbdyn::surface_to_sch(*(envSurface.get()), -0.001, 8)),
  pair(robotSch, envSch)
{
}

double SimulationContactPair::update(const mc_rbdyn::Robot & robot, const mc_rbdyn::Robot & env)
{
  updateSCH(robotSch, robot, robotSurface);
  updateSCH(envSch, env, envSurface);
  return pair.getDistance();
}

void SimulationContactPair::updateSCH(sch::S_Object * obj, const mc_rbdyn::Robot & robot, const std::shared_ptr<mc_rbdyn::Surface> & robotSurface)
{
  sch::transform(*obj, robot.mbc->bodyPosW[robot.bodyIndexByName(robotSurface->bodyName())]);
}

SimulationContactSensor::SimulationContactSensor(const std::vector<mc_rbdyn::Stance> & stances)
{
  std::vector<mc_rbdyn::Contact> cts;
  for(const auto & s : stances)
  {
    for(const auto & c : s.geomContacts)
    {
      bool found = false;
      for(const auto & ci : cts)
      {
        found = (ci.robotSurface->name() == c.robotSurface->name() and ci.envSurface->name() == c.envSurface->name());
        if(found) break;
      }
      if(!found)
      {
        cts.push_back(c);
      }
    }
  }
  for(const auto & c : cts)
  {
    surfacePairs.push_back(SimulationContactPair(c.robotSurface, c.envSurface));
  }
}

std::vector<std::string> SimulationContactSensor::update(MCController & ctl)
{
  std::vector<std::string> res;

  for(auto & p : surfacePairs)
  {
    double dist = p.update(ctl.robot(), ctl.env());
    if(dist < 0)
    {
      res.push_back(p.robotSurface->name());
    }
    res.push_back(p.robotSurface->name() + "_back");
  }

  return res;
}

}
