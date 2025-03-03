#pragma once
#include "PayloadOSModelSim.h"

namespace PayloadOS{
    namespace Simulation{
        namespace Model{
            class Inactive : public SimState{
            public:
                void init() override;
                UpdateParams update() override;
                void end() override;

            //singleton implementation
            private:
                Inactive();
            public:
                static Inactive* get();
                Inactive(const Inactive&) = delete;
                void operator=(const Inactive&) = delete;
            };

            class Delay : public SimState{
            public:
                void init() override;
                UpdateParams update() override;
                void end() override;

            //singleton implementation
            private:
                Delay();
            public:
                static Delay* get();
                Delay(const Delay&) = delete;
                void operator=(const Delay&) = delete;
            };

            class Boost : public SimState{
            public:
                void init() override;
                UpdateParams update() override;
                void end() override;

            //singleton implementation
            private:
                Boost();
            public:
                static Boost* get();
                Boost(const Boost&) = delete;
                void operator=(const Boost&) = delete;
            };

            class Coast : public SimState{
            public:
                void init() override;
                UpdateParams update() override;
                void end() override;

            //singleton implementation
            private:
                Coast();
            public:
                static Coast* get();
                Coast(const Coast&) = delete;
                void operator=(const Coast&) = delete;
            };

            class Apogee : public SimState{
            public:
                void init() override;
                UpdateParams update() override;
                void end() override;

            //singleton implementation
            private:
                Apogee();
            public:
                static Apogee* get();
                Apogee(const Apogee&) = delete;
                void operator=(const Apogee&) = delete;
            };

            class Drogue : public SimState{
            public:
                void init() override;
                UpdateParams update() override;
                void end() override;

            //singleton implementation
            private:
                Drogue();
            public:
                static Drogue* get();
                Drogue(const Drogue&) = delete;
                void operator=(const Drogue&) = delete;
            };

            class Deploy : public SimState{
            public:
                void init() override;
                UpdateParams update() override;
                void end() override;

            //singleton implementation
            private:
                Deploy();
            public:
                static Deploy* get();
                Deploy(const Deploy&) = delete;
                void operator=(const Deploy&) = delete;
            };

            class Main : public SimState{
            public:
                void init() override;
                UpdateParams update() override;
                void end() override;

            //singleton implementation
            private:
                Main();
            public:
                static Main* get();
                Main(const Main&) = delete;
                void operator=(const Main&) = delete;
            };

            class Landing : public SimState{
            public:
                void init() override;
                UpdateParams update() override;
                void end() override;

            //singleton implementation
            private:
                Landing();
            public:
                static Landing* get();
                Landing(const Landing&) = delete;
                void operator=(const Landing&) = delete;
            };

            class Landed : public SimState{
            public:
                void init() override;
                UpdateParams update() override;
                void end() override;

            //singleton implementation
            private:
                Landed();
            public:
                static Landed* get();
                Landed(const Landed&) = delete;
                void operator=(const Landed&) = delete;
            };
        }
    }
}