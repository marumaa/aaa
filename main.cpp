#include "Memory.hpp"
#include "Offsets.hpp"
#include <thread>
#include <unistd.h>

#include <array>
#include <iostream>

int main(){
    nig:
    if (getuid()) {
        std::cout << "[!]Run as sudo" << std::endl;
        return -1;
    }

    Memory::GetPID();

    while (1){
        for (int Index = 0; Index < 64; Index++){
            long EnemyPointer = Memory::Read<long>(OFF_REGION + OFF_ENTITY_LIST + ((Index + 1) << 5));
            long MyselfPointer = Memory::Read<long>(OFF_REGION + OFF_LOCAL_PLAYER);

            if (EnemyPointer == 0 || MyselfPointer == 0)
                continue;
            
            std::string Name = Memory::ReadString(EnemyPointer + OFF_NAME);

            if (Name != "player")
                continue;

            float WorldTime = Memory::Read<float>(MyselfPointer + OFF_TIME_BASE);
            float Time1 = Memory::Read<float>(EnemyPointer + OFF_LAST_VISIBLE_TIME);
            bool IsVisible = (Time1 + 0.2) >= WorldTime;

            Memory::Write<int>(EnemyPointer + OFF_GLOW_ENABLE, 1);
            Memory::Write<int>(EnemyPointer + OFF_GLOW_THROUGH_WALL,1);

            int GlowRadius;

            std::array<unsigned char, 4> highlightFunctionBits = {
                139,             // InsideFunction                            2
                80,            // OutlineFunction: HIGHLIGHT_OUTLINE_OBJECTIVE            125
                32, // OutlineRadius: size * 255 / 8                64
                64                          // (EntityVisible << 6) | State & 0x3F | (AfterPostProcess << 7)     64
            };

            std::array<float, 3> glowColorRGB = { 0.91, 0.26, 0.68 };

            static const int contextId = 0; 
            int settingIndex = 63;

            Memory::Write<unsigned char>(EnemyPointer + OFF_GLOW_HIGHLIGHT_ID + contextId, settingIndex);
            long highlightSettingsPtr = Memory::Read<long>(OFF_REGION + OFF_GLOW_HIGHLIGHTS);

            Memory::Write<typeof(highlightFunctionBits)>(highlightSettingsPtr + OFF_HIGHLIGHT_TYPE_SIZE * settingIndex + 0, highlightFunctionBits);
            Memory::Write<typeof(glowColorRGB)>(highlightSettingsPtr + OFF_HIGHLIGHT_TYPE_SIZE * settingIndex + 4, glowColorRGB);
            Memory::Write<int>(EnemyPointer + OFF_GLOW_FIX, 0);
            Memory::Write<int>(EnemyPointer + 0x264, 1000);  //DISTANCE
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}