//#include "inwanderland.h"
//#include "warriorproductor.h"
//#include "bleedingcomponent.h"
//#include "knockbackcomponent.h"
//#include "meleeattacksystem.h"
//#include "warriorskillsystem.h"
//#include "tauntingcomponent.h"
//#include "debugmeshes.h"
//#include "healerproductor.h"
//#include "magicianproductor.h"
//#include "singlenavigationfield.h"
//#include "unitdata.h"
//#include "playerunit.h"
//#include "robinskilldevelopmentsystem.h"
//#include "skillpreviewsystem.h"
//
//void warriorproductor::setunitdata()
//{
//    m_objectname = "warrior";
//    m_unittype = unit::unittype::warrior;
//    m_unitside = unit::unitside::player;
//
//    m_maxhealth = 250;
//    m_manapoint = 100;
//
//    m_autoattackdamage = 10;
//    m_criticalhitprobability = 0.2f;
//    m_criticalhitmultiplier = 1.5f;
//
//    m_defensepoint = 15;
//    m_dodgeprobability = 0.2f;
//    m_criticaldamagedecreasemultiplier = 0.2f;
//
//    m_maxaggronumber = 2;
//
//    m_idradius = 4.0f * unit_length;
//    m_atkradius = 1.7f * unit_length;
//    m_unitspeed = 4.5f;
//
//    m_attackdelay = 1.0f;
//
//    m_navfield = &singlenavigationfield::instance();
//
//    m_unitfbxname = "skm_robin";
//}
//
//void warriorproductor::singletoninitializer()
//{
//    setunitdata();
//}
//
//unit* warriorproductor::createunit(vector3d startpos)
//{
//#pragma region animation related member setting
//    m_unitgameobject = yunutyengine::scene::getcurrentscene()->addgameobjectfromfbx("skm_robin");
//    m_unitgameobject->gettransform()->setworldposition(startpos);
//
//    /// unitcomponent 추가
//    m_unitcomponent = m_unitgameobject->addcomponent<playerunit>();
//    robinskilldevelopmentsystem::instance().setownerunit(m_unitcomponent);
//
//    unitproductor::setunitcomponentmembers();
//#pragma endregion
//
//#pragma region auto attack setting (including passive logic)
//    auto bleedingsystemobject = yunutyengine::scene::getcurrentscene()->addgameobject();
//    auto bleedingsystem = bleedingsystemobject->addcomponent<bleedingcomponent>();
//    auto warriorattacksystem = m_unitgameobject->addcomponent<meleeattacksystem>();
//
//    robinskilldevelopmentsystem::instance().setrobinpassivecomponent(bleedingsystem);
//
//    warriorattacksystem->setmeleeattacktype(meleeattacktype::directattack);
//    warriorattacksystem->setownerunitobject(m_unitgameobject);
//    warriorattacksystem->setdirectattackspecialeffect(bleedingsystem);
//    warriorattacksystem->setdamage(m_autoattackdamage);
//#pragma endregion
//
//#pragma region q skill setting
//    m_qskillradius = 3.0f * unit_length;
//
//    auto qskillknockbackobject = yunutyengine::scene::getcurrentscene()->addgameobject();
//
//    auto qskillcollidercomponent = qskillknockbackobject->addcomponent<physics::spherecollider>();
//    qskillcollidercomponent->setradius(m_qskillradius);
//
//    auto qskillcolliderdebugobject = yunutyengine::scene::getcurrentscene()->addgameobject();
//    attachdebugmesh(qskillcolliderdebugobject, debugmeshtype::sphere, yunugi::color::red(), true);
//    qskillcolliderdebugobject->gettransform()->setlocalscale({ m_qskillradius * 2, m_qskillradius * 2, m_qskillradius * 2 });
//
//    auto knockbackcomponent = qskillknockbackobject->addcomponent<knockbackcomponent>();
//    knockbackcomponent->setskillownerunit(m_unitcomponent);
//#pragma endregion
//
//#pragma region w skill setting
//    m_wskillradius = 3.0f * unit_length;
//
//    auto wskillcolliderobject = yunutyengine::scene::getcurrentscene()->addgameobject();
//    auto wskillcollidercomponent = wskillcolliderobject->addcomponent<physics::spherecollider>();
//    wskillcollidercomponent->setradius(m_wskillradius);
//    auto wskilldamagecomponent = wskillcolliderobject->addcomponent<tauntingcomponent>();
//    wskilldamagecomponent->setskillownerunit(m_unitcomponent);
//    wskilldamagecomponent->setskilldamage(10.0f);
//
//    auto wskillcolliderdebugobject = yunutyengine::scene::getcurrentscene()->addgameobject();
//    attachdebugmesh(wskillcolliderdebugobject, debugmeshtype::sphere, yunugi::color::green(), true);
//    wskillcolliderdebugobject->gettransform()->setlocalscale({ m_wskillradius * 2, m_wskillradius * 2, m_wskillradius * 2 });
//#pragma endregion
//    // warrior skillsystem
//    auto warriorskillsystem = m_unitgameobject->addcomponent<warriorskillsystem>();
//    warriorskillsystem->setqskillknockbackobject(qskillknockbackobject);
//    warriorskillsystem->setwskillobject(wskillcolliderobject);
//    warriorskillsystem->setknockbackdebugobject(qskillcolliderdebugobject, m_qskillradius);
//    warriorskillsystem->setwskilldebugobject(wskillcolliderdebugobject, m_wskillradius);
//    warriorskillsystem->qskillradialoverlay = uimanager::instance().getuielementbyenum(uienumid::charinfo_robin)
//        ->getlocaluisbyenumid().at(uienumid::charinfo_skill_use_q_overlay);
//    warriorskillsystem->qskillcooltimenumberui = uimanager::instance().getuielementbyenum(uienumid::charinfo_robin)
//        ->getlocaluisbyenumid().at(uienumid::charinfo_skill_use_q_cooltime_number);
//    warriorskillsystem->eskillradialoverlay = uimanager::instance().getuielementbyenum(uienumid::charinfo_robin)
//        ->getlocaluisbyenumid().at(uienumid::charinfo_skill_use_w_overlay);
//    warriorskillsystem->eskillcooltimenumberui = uimanager::instance().getuielementbyenum(uienumid::charinfo_robin)
//        ->getlocaluisbyenumid().at(uienumid::charinfo_skill_use_w_cooltime_number);
//
//    float warriorskillonerange = 5.0f * unit_length;
//    warriorskillsystem->setskillonerange(warriorskillonerange);
//    warriorskillsystem->setskilltworange(0.0f);
//
//    robinskilldevelopmentsystem::instance().setskillsystemcomponent(warriorskillsystem);
//
//    unitproductor::addrangesystemcomponent();
//    unitproductor::addcollidercomponent();
//    unitproductor::addnavigationcomponent();
//    unitproductor::adddotweencomponent();
//    unitproductor::setplayerrelatedcomponents();
//
//    //skillpreviewsystem::instance().setdefaultskillrange(m_unitcomponent, unit::skillenum::q, warriorskillonerange);
//    //skillpreviewsystem::instance().setdefaultskillrange(m_unitcomponent, unit::skillenum::w, 0.0f);
//
//    auto rsrcmanager = yunutyengine::graphics::renderer::singleinstance().getresourcemanager();
//    auto animator = m_unitgameobject->getcomponent<yunutyengine::graphics::animator>();
//    auto& animlist = rsrcmanager->getanimationlist();
//    for (auto each : animlist)
//    {
//        if (each->getname() == l"rig_robin_arpbob|ani_robin_idle")
//        {
//            m_baseunitanimations.m_idleanimation = each;
//            m_baseunitanimations.m_idleanimation->setloop(true);
//            animator->pushanimation(m_baseunitanimations.m_idleanimation);
//            animator->play(m_baseunitanimations.m_idleanimation);
//        }
//        else if (each->getname() == l"rig_robin_arpbob|ani_robin_walk")
//        {
//            m_baseunitanimations.m_walkanimation = each;
//            m_baseunitanimations.m_walkanimation->setloop(true);
//            animator->pushanimation(m_baseunitanimations.m_walkanimation);
//        }
//        else if (each->getname() == l"rig_robin_arpbob|ani_robin_attack")
//        {
//            m_baseunitanimations.m_attackanimation = each;
//            m_baseunitanimations.m_attackanimation->setloop(false);
//        }
//        else if (each->getname() == l"rig_robin_arpbob|ani_robin_battlemode")
//        {
//            m_baseunitanimations.m_paralysisanimation = each;
//            m_baseunitanimations.m_paralysisanimation->setloop(false);
//            animator->pushanimation(m_baseunitanimations.m_paralysisanimation);
//        }
//        else if (each->getname() == l"rig_robin_arpbob|ani_robin_death")
//        {
//            m_baseunitanimations.m_deathanimation = each;
//            m_baseunitanimations.m_deathanimation->setloop(false);
//            animator->pushanimation(m_baseunitanimations.m_deathanimation);
//        }
//        /// skill animation
//        else if (each->getname() == l"rig_robin_arpbob|ani_robin_skill1")
//        {
//            each->setloop(true);
//            animator->pushanimation(each);
//            m_baseunitanimations.m_skilloneanimation = each;
//            m_unitcomponent->registerskillanimation(unit::skillenum::q, each);
//        }
//        else if (each->getname() == l"rig_robin_arpbob|ani_robin_skill2")
//        {
//            each->setloop(true);
//            animator->pushanimation(each);
//            m_baseunitanimations.m_skilltwoanimation = each;
//            m_unitcomponent->registerskillanimation(unit::skillenum::w, each);
//        }
//        else if (each->getname() == l"rig_robin_arpbob|ani_robin_battlestart")
//        {
//            m_baseunitanimations.m_battleengageanimation = each;
//            m_baseunitanimations.m_battleengageanimation->setloop(false);
//        }
//    }
//    m_unitcomponent->unitanimations = m_baseunitanimations;
//    setunitskillfunctiontoanimation();
//
//    return m_unitcomponent;
//}
//
//// 전략 패턴을 설명하기 위한 예시 코드
//// 전략 패턴의 핵심은 객체의 동작을 전략 객체를 통해 바꿀 수 있다는 것.
//// 아래의 경우 unitproductionorder가 전략에 해당되며, 이 전략 객체가 바뀜에 따라 makesome 동작이 바뀐다.
////class someone
////{
////	// 아래가 전략!
////	unitproductionorder* currentstrategy;
////	void makesome()
////	{
////		currentstrategy->createunitwithorder();
////	}
////	void changestrategy(unitproductionorder* newstrategy)
////	{
////		currentstrategy = newstrategy;
////	}
////	static void samplecode()
////	{
////		someone somedude;
////		somedude.changestrategy(new warriorproduction);
////		somedude.makesome();
////		somedude.changestrategy(new magicianproduction);
////		somedude.makesome();
////		somedude.changestrategy(new healerproduction);
////		somedude.makesome();
////	}
////};