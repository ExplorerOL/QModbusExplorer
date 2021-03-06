// Copyright (c) 2013, Razvan Petru
// All rights reserved.

// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:

// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice, this
//   list of conditions and the following disclaimer in the documentation and/or other
//   materials provided with the distribution.
// * The name of the contributors may not be used to endorse or promote products
//   derived from this software without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef QSLOGDEST_H
#define QSLOGDEST_H

#include "QsLogLevel.h"
#include <QSharedPointer>
#include <QtGlobal>
class QString;

namespace QsLogging
{

class Destination
{
public:
    typedef void (*LogFunction)(const QString &message, Level level);

public:
    virtual ~Destination(){}
    virtual void write(const QString& message, Level level) = 0;
    virtual bool isValid() = 0; // returns whether the destination was created correctly
};
typedef QSharedPointer<Destination> DestinationPtr;

//! Creates logging destinations/sinks. The caller will have ownership of 
//! the newly created destinations.
class DestinationFactory
{
public:
    static DestinationPtr MakeFileDestination(const QString& filePath, bool enableRotation = false, qint64 sizeInBytesToRotateAfter = 0, int oldLogsToKeep = 0);
    static DestinationPtr MakeDebugOutputDestination();
    // takes a pointer to a function
    static DestinationPtr MakeFunctorDestination(Destination::LogFunction f);
    // takes a QObject + signal/slot
    static DestinationPtr MakeFunctorDestination(QObject *receiver, const char *member);
};

} // end namespace

#endif // QSLOGDEST_H
