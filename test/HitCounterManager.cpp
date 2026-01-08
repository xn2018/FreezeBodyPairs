#include <Common/HitCounterManager.h>

#include <catch.hpp>

using namespace Common;

TEST_CASE("HitCounterManager/GetCountForNullActor") {
    CHECK(!HitCounterManager::GetSingleton().GetHitCount(nullptr).has_value());
}
