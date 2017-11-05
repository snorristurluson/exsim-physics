# exsim-physics
[![Build status](https://travis-ci.org/snorristurluson/exsim-physics.svg?branch=master)](https://travis-ci.org/snorristurluson/exsim-physics/builds#)

The physics engine for [exsim](https://github.com/snorristurluson/exsim) - large scale simulation done in Elixir.

Uses [Bullet Physics SDK](https://github.com/bulletphysics/bullet3)
and [RapidJSON](http://rapidjson.org/index.html).

This is really a thin wrapper around Bullet Physics, with a
command handler that reads in commands in JSON format from
a TCP socket.

Multiple connections are supported, with one master connection
and multiple viewer connections. If the master connection is
lost, the simulation is reset. All responses are sent to all
open connections.
