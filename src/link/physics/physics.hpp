#pragma once

#include <btBulletDynamicsCommon.h>
#include "link/singleton.hpp"

namespace link
{
    struct Physics : Singleton<Physics>
    {
        Physics() {}

        void init()
        {
            ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
            collisionConfiguration = new btDefaultCollisionConfiguration();

            ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
            dispatcher = new btCollisionDispatcher(collisionConfiguration);

            ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
            overlappingPairCache = new btDbvtBroadphase();

            ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
            solver = new btSequentialImpulseConstraintSolver;

            dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
            dynamicsWorld->setGravity(btVector3(0, -10, 0));
        }

        void add_rigidbody(btRigidBody* body)
        {
            dynamicsWorld->addRigidBody(body);
        }
        
        void update()
        {
            dynamicsWorld->stepSimulation(1.f / 60.f, 10);
        }

        void shutdown()
        {
            //remove the rigidbodies from the dynamics world and delete them
            for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
            {
                btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
                btRigidBody* body = btRigidBody::upcast(obj);
                if (body && body->getMotionState())
                {
                    delete body->getMotionState();
                }
                dynamicsWorld->removeCollisionObject(obj);
                delete obj;
            }

            ////delete collision shapes
            //for (int j = 0; j < collisionShapes.size(); j++)
            //{
            //    btCollisionShape* shape = collisionShapes[j];
            //    collisionShapes[j] = 0;
            //    delete shape;
            //}

            //delete dynamics world
            delete dynamicsWorld;

            //delete solver
            delete solver;

            //delete broadphase
            delete overlappingPairCache;

            //delete dispatcher
            delete dispatcher;

            delete collisionConfiguration;

            //next line is optional: it will be cleared by the destructor when the array goes out of scope
            //collisionShapes.clear();
        }


    private:
        btDefaultCollisionConfiguration* collisionConfiguration;
        btCollisionDispatcher* dispatcher;
        btBroadphaseInterface* overlappingPairCache;
        btSequentialImpulseConstraintSolver* solver;
        btDiscreteDynamicsWorld* dynamicsWorld;
    };
}

#define LINK_PHYSICS link::Physics::get()