//  Created by Emil Ernerfeldt on 2012-06-09.
//  Copyright (c) 2015 Emil Ernerfeldt AB. All rights reserved.
//

#include "random.hpp"

unsigned random_seed()
{
#if DEBUG
	return 9;
#else
	return arc4random();
#endif
}

namespace emath {

unsigned Random::s_seed = random_seed();
Random Random::s_random;

Random::Random(const char* seed) : Random((unsigned)std::hash<const char*>()(seed)) {
}

} // namespace emath
