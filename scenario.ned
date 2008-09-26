//
//    Copyright (C) 2008    Dmitry Ivanov <vonami@gmail.com>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

import
    "Host",
    "ChannelControl";


module Scenario
    parameters:
        playgroundSizeX: numeric const,
        playgroundSizeY: numeric const;
    submodules:
        channelcontrol: ChannelControl;
            parameters:
                playgroundSizeX = playgroundSizeX,
                playgroundSizeY = playgroundSizeY;
            display: "p=44,-46;i=misc/sun";
    host: Host[12];
             display: "i=device/wifilaptop_vs";

    connections nocheck:
endmodule

network scenario1 : Scenario
endnetwork

