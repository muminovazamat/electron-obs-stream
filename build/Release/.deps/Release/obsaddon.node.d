cmd_Release/obsaddon.node := ln -f "Release/obj.target/obsaddon.node" "Release/obsaddon.node" 2>/dev/null || (rm -rf "Release/obsaddon.node" && cp -af "Release/obj.target/obsaddon.node" "Release/obsaddon.node")
