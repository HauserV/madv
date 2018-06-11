/* Copyright 2018 Vojtěch Hauser <Vojtech.Hauser+dev@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * Distributions of all or part of the Software intended to be used by the
 * recipients as they would use the unmodified Software, containing
 * modifications that substantially alter, remove, or disable functionality of
 * the Software, outside of the documented configuration mechanisms provided by
 * the Software, shall be modified such that the Original Author's bug reporting
 * email addresses and urls are either replaced with the contact information of
 * the parties responsible for the changes, or removed entirely.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <iostream>
#include <ns3/system-thread.h>
#include <ns3/core-module.h>

#include "Protocol.h"
#include "Node.h"
#include "Point.h"
#include "WSN.h"
#include "PingPongProtocol.h"

int main(int argc, char *argv[])
{
    WSN wsn(150, 150);

    //Node* np = new Node({Protocol()}, 0);

    // Enable calculation of FCS in the trailers.
    ns3::GlobalValue::Bind("ChecksumEnabled", ns3::BooleanValue (true));

    //auto prots = {Protocol()};

    std::vector<std::unique_ptr<Protocol>> prots;
    prots.push_back(std::make_unique<PingPongProtocol>());

    std::vector<std::unique_ptr<Protocol>> prots2;
    prots2.push_back(std::make_unique<PingPongProtocol>());

    std::vector<std::unique_ptr<Node>> vec;
    vec.push_back(std::make_unique<Node>(std::move(prots)));
    vec.push_back(std::make_unique<Node>(std::move(prots2)));

    std::cout << *vec.front() << std::endl;

    wsn.place(std::move(vec));

    std::cout << *wsn.view_nodes().front() << std::endl;
    std::cout << **std::next(wsn.view_nodes().begin()) << std::endl;

    // Tracing
    for(auto& node_ptr : wsn.view_nodes())
    {
        node_ptr->init_protocols();
        node_ptr->enable_pcap("lr-wpan-data");
        node_ptr->enable_ascii("lr-wpan-data.tr", true);
    }

    wsn.view_nodes().front()->send_packet({0xab, 0xcd}, "ff:ff", ns3::Seconds(2));

    ns3::Simulator::Run();



    ns3::Simulator::Stop(ns3::Seconds(10));
    ns3::Simulator::Destroy();

    return 0;
}