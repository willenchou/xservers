
#include "multiplexing_impl.h"
namespace x{
	 IMultiPlexing* IMultiPlexing::newMultiPlexing(){
      return new MultiPlexing();
	}
}
