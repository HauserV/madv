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

#ifndef MADV_WSN_H
#define MADV_WSN_H

#include <memory>

#include <ns3/double.h>
#include <ns3/ptr.h>
#include <ns3/object.h>
#include <ns3/random-variable-stream.h>
#include <ns3/position-allocator.h>
#include <ns3/mobility-module.h>
#include <ns3/spectrum-channel.h>
#include <ns3/single-model-spectrum-channel.h>
#include <ns3/propagation-loss-model.h>
#include <ns3/propagation-delay-model.h>
#include <functional>

#include "Node.h"

class WSN
{
public:
    WSN(double max_x, double max_y)
    {
        // Deployment: randomly in a rectangle; todo extract
        auto position_allocator =
                ns3::CreateObject<ns3::RandomRectanglePositionAllocator>();

        auto random_x = ns3::CreateObject<ns3::UniformRandomVariable>();
        random_x->SetAttribute("Min", ns3::DoubleValue(0));
        random_x->SetAttribute("Max", ns3::DoubleValue(max_x));

        auto random_y = ns3::CreateObject<ns3::UniformRandomVariable>();
        random_y->SetAttribute("Min", ns3::DoubleValue(0));
        random_y->SetAttribute("Max", ns3::DoubleValue(max_y));

        position_allocator->SetX(random_x);
        position_allocator->SetY(random_y);

        this->position_allocator = position_allocator;

        // Channel model
        channel = ns3::CreateObject<ns3::SingleModelSpectrumChannel>();
        auto propagation =
                ns3::CreateObject<ns3::LogDistancePropagationLossModel>();
        auto delay =
                ns3::CreateObject<ns3::ConstantSpeedPropagationDelayModel>();
        channel->AddPropagationLossModel(propagation);
        channel->SetPropagationDelayModel(delay);
    }

    void place(std::vector<std::unique_ptr<Node>> nodes)
    {
        for (auto& node : nodes)
        {
            // PHY
            node->set_position(position_allocator->GetNext());
            node->set_channel(channel);

            // MAC
            node->set_address();
            node->set_pan();
        }

        this->nodes.insert(this->nodes.end(),
                           std::make_move_iterator(nodes.begin()),
                           std::make_move_iterator(nodes.end()));
    }

    const std::vector<std::unique_ptr<Node>>& view_nodes()
    {
        return nodes;
    }

private:
    std::vector<std::unique_ptr<Node>> nodes;

    ns3::Ptr<ns3::PositionAllocator> position_allocator;
    ns3::Ptr<ns3::SingleModelSpectrumChannel> channel;
};


#endif //MADV_WSN_H
