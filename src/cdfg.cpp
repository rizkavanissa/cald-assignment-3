#include "cdfg.h"

// Create the ALAP graph
void setALAPS(std::vector<Operation *> ops, int lat)
{
    int earliestSuc; // Earliest successor
    
    ops.at(1)->setALAPTime(lat + 1); // Plus one to account for the wait time
    ops.at(1)->setALAPDone(true);

    while (!ops.at(0)->isALAPDone())
    {
        for (int i = 0; i < ops.size(); i++)    // Iteratre over existing operations based on the number of operations stored
        {
            Operation *op = ops.at(i);  // Retrieve the operation at the current index of the loop

            if (!op->isALAPDone())
            {
                bool sucsALAPed = true;
                earliestSuc = lat + 1;

                for (int j = 0; j < op->successors.size(); j++)
                {
                    Operation *suc = op->getSucAt(j);

                    if (!(suc == op))
                    {
                        sucsALAPed &= suc->isALAPDone();

                        if (suc->ALAPTime < earliestSuc)
                        {
                            earliestSuc = suc->ALAPTime;
                        }
                    }
                }

                if (sucsALAPed)
                {
                    if (op->type == MUL)
                    {
                        op->setALAPTime(earliestSuc - 2);
                    }
                    else if (op->type == DIV || op->type == MOD)
                    {
                        op->setALAPTime(earliestSuc - 3);
                    }
                    else
                    {
                        op->setALAPTime(earliestSuc - 1);
                    }

                    op->setALAPDone(true);

                    if (op->ALAPTime < 0) // Check whether the ALAP time for an operation is negative (i.e., should have been scheduled in previous cycle)
                    {
                        std::cerr << "Error: Performed ALAP scheduling failed" << std::endl;
                    }
                }
            }
        }
    }
    return;
}

// Create the List-R scheduling
void List_R(std::vector<Operation *> ops, int lat)
{
    int preid;

    for (int count = 0; count < ops.size(); count++)
    {
        ops.at(count)->edge = 0;
        ops.at(count)->scheduledState = 0;
        ops.at(count)->scheduledTime = -1;
    }

    for (int step = 2; step < ops.size(); step++)
    {
        for (int sucid = 0; sucid < ops.at(step)->successors.size(); sucid++)
        {
            preid = ops.at(step)->getSucAt(sucid)->id;
            ops.at(preid + 1)->edge += 1;
        }
    }

    int needed[Resource::INVALID_RESOURCE] = {1, 1, 1, 1};
    int used[Resource::INVALID_RESOURCE];

    // "Schedule In NOP"
    std::vector<Operation *> listing = ops;
    listing.erase(listing.begin(), listing.begin() + 2);

    std::vector<Operation *> unschedulingadd;   // Store unscheduled adder/subtractor
    std::vector<Operation *> unschedulingmul;   // Store unscheduled multiplier
    std::vector<Operation *> unschedulinglog;   // Store unscheduled logical (comparisons, shift, and ternary operators)
    std::vector<Operation *> unschedulingdid;   // Store unscheduled divider (division & modulo)

    std::vector<Operation *> schedulingadd;     // Store scheduled adder/subtractor
    std::vector<Operation *> schedulingmul;     // Store scheduled multiplier
    std::vector<Operation *> schedulinglog;     // Store scheduled logical (comparisons, shift, and ternary operators)
    std::vector<Operation *> schedulingdid;     // Store scheduled divider (division & modulo)

    // Indicate whether either operators has been scheduled
    bool scheduledcomadd = false;
    bool scheduledcommul = false;
    bool scheduledcomdid = false;
    bool scheduledcomlog = false;
    bool scheduledcomif = false;

    bool ifexist = false;
    int ifstate = 0;

    int delnum;
    int waitnum;
    int deladd = 0, delmul = 0, deldid = 0, dellog = 0, delif = 0;

    // Keep track of how many resources are used
    used[Resource::ADDER] = 1;
    used[Resource::MULTIPLIER] = 1;
    used[Resource::LOGIC] = 1;
    used[Resource::DIVIDER] = 1;

    ifexist = false;

    for (int I = 1; I <= lat; I++)  // Perform List-R scheduling for as many cycles as the given latency constraint
    {
        // Reset the flag for eeach operation to false awaiting any scheduled operation
        scheduledcomadd = false;
        scheduledcommul = false;
        scheduledcomdid = false;
        scheduledcomlog = false;
        // cout << "V5 is " << listing.at(4)->scheduledTime << "\n";

        // Check schedule time, remove edge
        deladd = 0;                         // Adder
        delmul = 0;                         // Multplier
        dellog = 0;                         // Logical
        deldid = 0;                         // Divider

        // --------------------------   Adder (or Subtractor)    --------------------------------------

        for (int check1 = 0; check1 < schedulingadd.size(); check1++)
        {
            if (schedulingadd.at(check1)->scheduledTime == 0)
            {
                deladd++;
                int edgeid;

                for (int del = 0; del < schedulingadd.at(check1)->successors.size(); del++)
                {
                    edgeid = schedulingadd.at(check1)->getSucAt(del)->id - 1;
                    listing.at(edgeid)->edge -= 1;
                }
            }
        }

        if (deladd != 0)
        {
            for (int remove = 0; remove < deladd; remove++)
            {
                schedulingadd.erase(schedulingadd.begin());
            }
        }

        // --------------------------   Multiplier    --------------------------------------

        for (int check1 = 0; check1 < schedulingmul.size(); check1++)
        {
            if (schedulingmul.at(check1)->scheduledTime == 0)
            {
                deladd++;
                int edgeid;
                for (int del = 0; del < schedulingmul.at(check1)->successors.size(); del++)
                {
                    edgeid = schedulingmul.at(check1)->getSucAt(del)->id - 1;
                    listing.at(edgeid)->edge -= 1;
                }
            }
        }

        if (delmul != 0)
        {
            for (int remove = 0; remove < delmul; remove++)
            {
                schedulingmul.erase(schedulingmul.begin());
            }
        }

        // --------------------------   Logical    --------------------------------------

        for (int check1 = 0; check1 < schedulinglog.size(); check1++)
        {
            if (schedulinglog.at(check1)->scheduledTime <= 0)
            {
                dellog++;
                int edgeid;

                for (int del = 0; del < schedulinglog.at(check1)->successors.size(); del++)
                {
                    edgeid = schedulinglog.at(check1)->getSucAt(del)->id - 1;
                    listing.at(edgeid)->edge -= 1;
                }
            }
        }

        if (dellog != 0)
        {
            for (int remove = 0; remove < dellog; remove++)
            {
                schedulinglog.erase(schedulinglog.begin());
            }
        }

        // --------------------------   Divider (or Modulo)    --------------------------------------

        for (int check1 = 0; check1 < schedulingdid.size(); check1++)
        {
            if (schedulingdid.at(check1)->scheduledTime == 0)
            {
                deldid++;
                int edgeid;
                for (int del = 0; del < schedulingdid.at(check1)->successors.size(); del++)
                {
                    edgeid = schedulingdid.at(check1)->getSucAt(del)->id - 1;
                    listing.at(edgeid)->edge -= 1;
                }
            }
        }

        if (deldid != 0)
        {
            for (int remove = 0; remove < deldid; remove++)
            {
                schedulingdid.erase(schedulingdid.begin());
            }
        }

        // ----------------------------------------------------------------

        //  Check edge if is 0;
        for (int count1 = 0; count1 < listing.size(); count1++)
        {
            if (listing.at(count1)->edge == 0)
            {

                if (listing.at(count1)->type == MUL)
                {
                    unschedulingmul.push_back(listing.at(count1));
                }
                else if (listing.at(count1)->type == ADD || listing.at(count1)->type == SUB)
                {
                    unschedulingadd.push_back(listing.at(count1));
                }
                else if (listing.at(count1)->type == COMP || listing.at(count1)->type == MUX || listing.at(count1)->type == SHR || listing.at(count1)->type == SHL)
                {
                    unschedulinglog.push_back(listing.at(count1));
                }
                else if (listing.at(count1)->type == IF)
                {
                    ifexist = true;
                    ifstate = count1;
                    listing.at(count1)->scheduledTime = 0;
                    if (count1 == 0)
                    {
                        ops.at(count1 + 2)->scheduledState = I;
                        int edgeid;
                        for (int del = 0; del < listing.at(count1)->successors.size(); del++)
                        {
                            edgeid = listing.at(count1)->getSucAt(del)->id - 1;
                            listing.at(edgeid)->edge -= 1;
                        }
                        listing.at(count1)->edge = -1;
                    }
                    else if (listing.at(count1 - 1)->scheduledTime == 0)
                    {
                        ops.at(count1 + 2)->scheduledState = I;
                        int edgeid;
                        for (int del = 0; del < listing.at(count1)->successors.size(); del++)
                        {
                            edgeid = listing.at(count1)->getSucAt(del)->id - 1;
                            listing.at(edgeid)->edge -= 1;
                        }
                        listing.at(count1)->edge = -1;
                    }
                }
                else if (listing.at(count1)->type == ELSE)
                {
                    if (ifexist)
                    {
                        listing.at(count1)->scheduledTime = 0;
                        if (listing.at(count1 - 1)->scheduledTime == 0)
                        {
                            listing.at(count1)->scheduledTime = 0;
                            ops.at(count1 + 2)->scheduledState = (ifstate + 1);
                            int edgeid;
                            for (int del = 0; del < listing.at(count1)->successors.size(); del++)
                            {
                                edgeid = listing.at(count1)->getSucAt(del)->id - 1;
                                listing.at(edgeid)->edge -= 1;
                            }
                            listing.at(count1)->edge = -1;
                            ifexist = false;
                        }
                    }
                }
                else
                {
                    unschedulingdid.push_back(listing.at(count1));
                }
                if (listing.at(count1)->type != IF && listing.at(count1)->type != ELSE)
                {
                    listing.at(count1)->edge = -1;
                }
            }
        }

        // --------------------------------------- Schedule for Operations --------------------------------

        // ███████╗ ██████╗██╗  ██╗███████╗██████╗ ██╗   ██╗██╗     ███████╗     █████╗ ██████╗ ██████╗     ██╗███████╗██╗   ██╗██████╗ 
        // ██╔════╝██╔════╝██║  ██║██╔════╝██╔══██╗██║   ██║██║     ██╔════╝    ██╔══██╗██╔══██╗██╔══██╗   ██╔╝██╔════╝██║   ██║██╔══██╗
        // ███████╗██║     ███████║█████╗  ██║  ██║██║   ██║██║     █████╗      ███████║██║  ██║██║  ██║  ██╔╝ ███████╗██║   ██║██████╔╝
        // ╚════██║██║     ██╔══██║██╔══╝  ██║  ██║██║   ██║██║     ██╔══╝      ██╔══██║██║  ██║██║  ██║ ██╔╝  ╚════██║██║   ██║██╔══██╗
        // ███████║╚██████╗██║  ██║███████╗██████╔╝╚██████╔╝███████╗███████╗    ██║  ██║██████╔╝██████╔╝██╔╝   ███████║╚██████╔╝██████╔╝
        // ╚══════╝ ╚═════╝╚═╝  ╚═╝╚══════╝╚═════╝  ╚═════╝ ╚══════╝╚══════╝    ╚═╝  ╚═╝╚═════╝ ╚═════╝ ╚═╝    ╚══════╝ ╚═════╝ ╚═════╝ 

        for (int count2 = 0; count2 < unschedulingadd.size(); count2++) // Iterate over the unscheduled queue of adder/subtractor operations
        {
            waitnum = unschedulingadd.at(count2)->ALAPTime - I; // Calculate slack time

            if (waitnum == 0) // Current adder/subtractor must be scheduled now since slack time is zero
            {
                unschedulingadd.at(count2)->scheduledTime = 1;                  // Assign cycle delay of adder/subtractor
                schedulingadd.push_back(unschedulingadd.at(count2));            // Push the adder/subtractor operation onto the scheduled queue
                used[ADDER] = schedulingadd.size();                             // Update the number of adder/subtractor resources used
                scheduledcomadd = true;                                         // Flag that a adder/subtractor operation is scheduled

                for (int count5 = 0; count5 < ops.size(); count5++) // Iterator through the existing operations
                {
                    if (unschedulingadd.at(count2)->id == ops.at(count5)->id) // Find the adder/subtrator operation
                    {
                        ops.at(count5)->scheduledState = (I);   // Set the current cycle time as the cycle time of the  adder/subtrator operation to be scheduled immediately
                    }
                }
            }
        }

        if (scheduledcomadd) // Check whether there is already a scheduled adder/subtrator in the current cycle time
        {
            for (int check2 = 2; check2 < ops.size(); check2++) // Iterate through the existing operations
            {
                if (ops.at(check2)->scheduledState != 0) // Check if the adder/subtrator operation is truly scheduled
                {
                    for (int check3 = 0; check3 < unschedulingadd.size(); check3++) // Iterate through the unscheduled list of multplier operations
                    {
                        if (unschedulingadd.at(check3)->id == ops.at(check2)->id) // Find the adder/subtrator operation
                        {
                            // Since the adder/subtrator operation has been scheduled remove it from the unscheduled list
                            unschedulingadd.erase(unschedulingadd.begin() + check3);
                        }
                    }
                }
            }
        }

        if (!scheduledcomadd && (unschedulingadd.size() > 0)) // Check whether there is no adder/subtrator scheduled yet in the current cycle time
        {
            // Check whether the number of adder/subtractor resources is the same as the number of scheduled adder/subtractor resources
            if (used[Resource::ADDER] >= schedulingadd.size())
            {
                int waitnum2 = lat;     // Store the latency constraint value
                int pointer = 0;        // Pointer to a adder/subtrator operation

                for (int count3 = 0; count3 < unschedulingadd.size(); count3++) //  Iterate through the unscheduled adder/subtrator operations
                {
                    waitnum = unschedulingadd.at(count3)->ALAPTime - I; // Calculate the slack time of the adder/subtrator operation in current index

                    // Check if the slack of the adder/subtrator operation at the current index is less than the total latecy cycle
                    // In other words, this adder/subtrator operation must be scheduled immediately
                    //
                    if (waitnum < waitnum2)
                    {
                        pointer = count3;
                        waitnum2 = waitnum;
                    }
                }

                unschedulingadd.at(pointer)->scheduledTime = 1; // Assign that adder/subtrator operation to the current latency cycle

                for (int count5 = 0; count5 < ops.size(); count5++) // Iterate through the vector of operations
                {
                    // Find that adder/subtrator operation pointed by pointer (that must be scheduled immediately)
                    if (unschedulingadd.at(pointer)->id == ops.at(count5)->id)
                    {
                        ops.at(count5)->scheduledState = (I);   // Assign that adder/subtrator operation to the current latency cycle
                    }
                }

                schedulingadd.push_back(unschedulingadd.at(pointer));       // Push the pointed adder/subtrator operation onto the scheduled queue
                unschedulingadd.erase(unschedulingadd.begin() + pointer);   // Remove the pointed adder/subtrator operation from the unscheduled queue
            }
        }


        // ███████╗ ██████╗██╗  ██╗███████╗██████╗ ██╗   ██╗██╗     ███████╗    ███╗   ███╗██╗   ██╗██╗  ████████╗██╗██████╗ ██╗     ██╗███████╗██████╗ 
        // ██╔════╝██╔════╝██║  ██║██╔════╝██╔══██╗██║   ██║██║     ██╔════╝    ████╗ ████║██║   ██║██║  ╚══██╔══╝██║██╔══██╗██║     ██║██╔════╝██╔══██╗
        // ███████╗██║     ███████║█████╗  ██║  ██║██║   ██║██║     █████╗      ██╔████╔██║██║   ██║██║     ██║   ██║██████╔╝██║     ██║█████╗  ██████╔╝
        // ╚════██║██║     ██╔══██║██╔══╝  ██║  ██║██║   ██║██║     ██╔══╝      ██║╚██╔╝██║██║   ██║██║     ██║   ██║██╔═══╝ ██║     ██║██╔══╝  ██╔══██╗
        // ███████║╚██████╗██║  ██║███████╗██████╔╝╚██████╔╝███████╗███████╗    ██║ ╚═╝ ██║╚██████╔╝███████╗██║   ██║██║     ███████╗██║███████╗██║  ██║
        // ╚══════╝ ╚═════╝╚═╝  ╚═╝╚══════╝╚═════╝  ╚═════╝ ╚══════╝╚══════╝    ╚═╝     ╚═╝ ╚═════╝ ╚══════╝╚═╝   ╚═╝╚═╝     ╚══════╝╚═╝╚══════╝╚═╝  ╚═╝

        for (int count2 = 0; count2 < unschedulingmul.size(); count2++) // Iterate over the unscheduled queue of multiplier operations
        {
            waitnum = unschedulingmul.at(count2)->ALAPTime - I; // Calculate slack time

            if (waitnum == 0)   // Current multiplier must be scheduled now since slack time is zero
            {
                unschedulingmul.at(count2)->scheduledTime = 2;                  // Assign cycle delay of multiplier
                schedulingmul.push_back(unschedulingmul.at(count2));            // Push the multiplier operation onto the scheduled queue
                used[Resource::MULTIPLIER] = schedulingmul.size();              // Update the number of multiplier resources used
                scheduledcommul = true;                                         // Flag that a multiplier operation is scheduled

                for (int count5 = 0; count5 < ops.size(); count5++) // Iterator through the existing operations
                {
                    if (unschedulingmul.at(count2)->id == ops.at(count5)->id) // Find the multiplier operation
                    {
                        ops.at(count5)->scheduledState = (I);   // Set the current cycle time as the cycle time of the  multiplier operation to be scheduled immediately
                    }
                }
            }
        }

        if (scheduledcommul) // Check whether there is already a scheduled multiplier in the current cycle time
        {
            for (int check2 = 2; check2 < ops.size(); check2++) // Iterate through the existing operations
            {
                if (ops.at(check2)->scheduledState != 0) // Check if the multiplier operation is truly scheduled
                {
                    for (int check3 = 0; check3 < unschedulingmul.size(); check3++) // Iterate through the unscheduled list of multplier operations
                    {
                        if (unschedulingmul.at(check3)->id == ops.at(check2)->id) // Find the scheduled multiplier operation in the unscheduled list
                        {
                            // Since the multiplier operation has been scheduled remove it from the unscheduled list
                            unschedulingmul.erase(unschedulingmul.begin() + check3);
                        }
                    }
                }
            }
        }

        if (!scheduledcommul && (unschedulingmul.size() > 0)) // Check whether there is no multiplier scheduled yet in the current cycle time
        {
            // Check whether the number of multiplier resources is the same as the number of scheduled multiplier resources
            if (used[Resource::MULTIPLIER] > schedulingmul.size())
            {
                int waitnum2 = lat;     // Store the latency constraint value
                int pointer = 0;        // Pointer to a multiplier operation

                for (int count3 = 0; count3 < unschedulingmul.size(); count3++) //  Iterate through the unscheduled multiplier operations
                {
                    waitnum = unschedulingmul.at(count3)->ALAPTime - I; // Calculate the slack time of the multiplier operation in current index

                    // Check if the slack of the multiplier operation at the current index is less than the total latecy cycle
                    // In other words, this multiplier operation must be scheduled immediately
                    //
                    if (waitnum < waitnum2)
                    {
                        pointer = count3;
                        waitnum2 = waitnum;
                    }
                }

                unschedulingmul.at(pointer)->scheduledTime = 2; // Set the cycle delay for the multiplier operation that must be scheduled immediately

                for (int count5 = 0; count5 < ops.size(); count5++) // Iterate through the vector of operations
                {
                    // Find that multiplier operation pointed by pointer (that must be scheduled immediately)
                    if (unschedulingmul.at(pointer)->id == ops.at(count5)->id)
                    {
                        ops.at(count5)->scheduledState = (I);   // Assign that multiplier operation to the current latency cycle
                    }
                }
                
                schedulingmul.push_back(unschedulingmul.at(pointer));       // Push the pointed multiplier operation onto the scheduled queue
                unschedulingmul.erase(unschedulingmul.begin() + pointer);   // Remove the pointed multiplier operation from the unscheduled queue
            }
        }


        // ███████╗ ██████╗██╗  ██╗███████╗██████╗ ██╗   ██╗██╗     ███████╗    ██╗      ██████╗  ██████╗ ██╗ ██████╗ █████╗ ██╗     
        // ██╔════╝██╔════╝██║  ██║██╔════╝██╔══██╗██║   ██║██║     ██╔════╝    ██║     ██╔═══██╗██╔════╝ ██║██╔════╝██╔══██╗██║     
        // ███████╗██║     ███████║█████╗  ██║  ██║██║   ██║██║     █████╗      ██║     ██║   ██║██║  ███╗██║██║     ███████║██║     
        // ╚════██║██║     ██╔══██║██╔══╝  ██║  ██║██║   ██║██║     ██╔══╝      ██║     ██║   ██║██║   ██║██║██║     ██╔══██║██║     
        // ███████║╚██████╗██║  ██║███████╗██████╔╝╚██████╔╝███████╗███████╗    ███████╗╚██████╔╝╚██████╔╝██║╚██████╗██║  ██║███████╗
        // ╚══════╝ ╚═════╝╚═╝  ╚═╝╚══════╝╚═════╝  ╚═════╝ ╚══════╝╚══════╝    ╚══════╝ ╚═════╝  ╚═════╝ ╚═╝ ╚═════╝╚═╝  ╚═╝╚══════╝

        for (int count2 = 0; count2 < unschedulinglog.size(); count2++) // Iterate over the unscheduled queue of logical operations
        {
            waitnum = unschedulinglog.at(count2)->ALAPTime - I; // Calculate slack time

            if (waitnum == 0)   // Current logical must be scheduled now since slack time is zero
            {
                unschedulinglog.at(count2)->scheduledTime = 1;                  // Assign cycle delay of logical
                schedulinglog.push_back(unschedulinglog.at(count2));            // Push the logical operation onto the scheduled queue
                used[LOGIC] = schedulinglog.size();                             // Update the number of logical resources used
                scheduledcomlog = true;                                         // Flag that a logical operation is scheduled

                for (int count5 = 0; count5 < ops.size(); count5++) // Iterator through the existing operations
                {
                    if (unschedulinglog.at(count2)->id == ops.at(count5)->id)   // Find the logical operation
                    {
                        ops.at(count5)->scheduledState = (I);   // Set the current cycle time as the cycle time of the  logical operation to be scheduled immediately
                    }
                }
            }
        }

        if (scheduledcomlog)    // Check whether there is already a scheduled logical in the current cycle time
        {
            for (int check2 = 2; check2 < ops.size(); check2++) // Iterate through the existing operations
            {
                if (ops.at(check2)->scheduledState != 0)    // Check if the logical operation is truly scheduled
                {
                    for (int check3 = 0; check3 < unschedulinglog.size(); check3++) // Iterate through the unscheduled list of multplier operations
                    {
                        if (unschedulinglog.at(check3)->id == ops.at(check2)->id)   // Find the scheduled logical operation in the unscheduled list
                        {
                            // Since the logical operation has been scheduled remove it from the unscheduled list
                            unschedulinglog.erase(unschedulinglog.begin() + check3);
                        }
                    }
                }
            }
        }

        if (!scheduledcomlog && (unschedulinglog.size() > 0))   // Check whether there is no logical scheduled yet in the current cycle time
        {
            // Check whether the number of logical resources is the same as the number of scheduled logical resources
            if (used[LOGIC] > schedulinglog.size()) 
            {
                int waitnum2 = lat;     // Store the latency constraint value
                int pointer = 0;        // Pointer to a logical operation

                for (int count3 = 0; count3 < unschedulinglog.size(); count3++) //  Iterate through the unscheduled logical operations
                {
                    waitnum = unschedulinglog.at(count3)->ALAPTime - I; // Calculate the slack time of the logical operation in current index

                    // Check if the slack of the logical operation at the current index is less than the total latecy cycle
                    // In other words, this logical operation must be scheduled immediately
                    //
                    if (waitnum < waitnum2)
                    {
                        pointer = count3;
                        waitnum2 = waitnum;
                    }
                }

                unschedulinglog.at(pointer)->scheduledTime = 1; // Set the cycle delay for the logical operation that must be scheduled immediately

                for (int count5 = 0; count5 < ops.size(); count5++) // Iterate through the vector of operations
                {
                    // Find that logical operation pointed by pointer (that must be scheduled immediately)
                    if (unschedulinglog.at(pointer)->id == ops.at(count5)->id)
                    {
                        ops.at(count5)->scheduledState = (I);   // Assign that logical operation to the current latency cycle
                    }
                }

                schedulinglog.push_back(unschedulinglog.at(pointer));   // Push the pointed logical operation onto the scheduled queue
                unschedulinglog.erase(unschedulinglog.begin() + pointer);   // Remove the pointed logical operation from the unscheduled queue
            }
        }


        // ███████╗ ██████╗██╗  ██╗███████╗██████╗ ██╗   ██╗██╗     ███████╗    ██████╗ ██╗██╗   ██╗██╗██████╗ ███████╗██████╗ 
        // ██╔════╝██╔════╝██║  ██║██╔════╝██╔══██╗██║   ██║██║     ██╔════╝    ██╔══██╗██║██║   ██║██║██╔══██╗██╔════╝██╔══██╗
        // ███████╗██║     ███████║█████╗  ██║  ██║██║   ██║██║     █████╗      ██║  ██║██║██║   ██║██║██║  ██║█████╗  ██████╔╝
        // ╚════██║██║     ██╔══██║██╔══╝  ██║  ██║██║   ██║██║     ██╔══╝      ██║  ██║██║╚██╗ ██╔╝██║██║  ██║██╔══╝  ██╔══██╗
        // ███████║╚██████╗██║  ██║███████╗██████╔╝╚██████╔╝███████╗███████╗    ██████╔╝██║ ╚████╔╝ ██║██████╔╝███████╗██║  ██║
        // ╚══════╝ ╚═════╝╚═╝  ╚═╝╚══════╝╚═════╝  ╚═════╝ ╚══════╝╚══════╝    ╚═════╝ ╚═╝  ╚═══╝  ╚═╝╚═════╝ ╚══════╝╚═╝  ╚═╝

        for (int count2 = 0; count2 < unschedulingdid.size(); count2++) // Iterate over the unscheduled queue of divider/modulo operations
        {
            waitnum = unschedulingdid.at(count2)->ALAPTime - I; // Calculate slack time

            if (waitnum == 0) // Current divider/modulo must be schedule now since slack time is zero
            {
                unschedulingdid.at(count2)->scheduledTime = 3;              // Assign cycle delay of divider/modulo
                schedulingdid.push_back(unschedulingdid.at(count2));        // Push the divider/modulo operation onto the scheduled queue
                used[Resource::DIVIDER] = schedulingdid.size();             // Update the number of divider/modulo resources used
                scheduledcomdid = true;                                     // Flag that a divider/modulo operation is scheduled

                for (int count5 = 0; count5 < ops.size(); count5++) // Iterator through the existing operations
                {
                    if (unschedulingdid.at(count2)->id == ops.at(count5)->id)   // Find the divider/modulo operation
                    {
                        ops.at(count5)->scheduledState = (I);   // Set the current cycle time as the cycle time of the  divider/modulo operation to be scheduled immediately
                    }
                }
            }
        }

        if (scheduledcomdid)    // Check whether there is already a scheduled divider/modulo in the current cycle time
        {
            for (int check2 = 2; check2 < ops.size(); check2++) // Iterate through the existing operations
            {
                if (ops.at(check2)->scheduledState != 0)    // Check if the divider/modulo operation is truly scheduled
                {
                    for (int check3 = 0; check3 < unschedulingdid.size(); check3++) // Iterate through the unscheduled list of multplier operations
                    {
                        if (unschedulingdid.at(check3)->id == ops.at(check2)->id)   // Find the scheduled divider/modulo operation in the unscheduled list
                        {
                            // Since the divider/modulo operation has been scheduled remove it from the unscheduled list
                            unschedulingdid.erase(unschedulingdid.begin() + check3);
                        }
                    }
                }
            }
        }

        if (!scheduledcomdid && (unschedulingdid.size() > 0))   // Check whether there is no divider/modulo scheduled yet in the current cycle time
        {
            // Check whether the number of divider/modulo resources is the same as the number of scheduled divider/modulo resources
            if (used[Resource::DIVIDER] > schedulingdid.size())
            {
                int waitnum2 = lat;         // Store the latency constraint value
                int pointer = 0;            // Pointer to a divider/modulo operation
                
                for (int count3 = 0; count3 < unschedulingdid.size(); count3++) //  Iterate through the unscheduled divider/modulo operations
                {
                    waitnum = unschedulingdid.at(count3)->ALAPTime - I; // Calculate the slack time of the divider/modulo operation in current index

                    // Check if the slack of the divider/modulo operation at the current index is less than the total latecy cycle
                    // In other words, this divider/modulo operation must be scheduled immediately
                    //
                    if (waitnum < waitnum2)
                    {
                        pointer = count3;
                        waitnum2 = waitnum;
                    }
                }

                unschedulingdid.at(pointer)->scheduledTime = 3; // Set the cycle delay for the divider/modulo operation that must be scheduled immediately

                for (int count5 = 0; count5 < ops.size(); count5++) // Iterate through the vector of operations
                {
                    // Find that divider/modulo operation pointed by pointer (that must be scheduled immediately)
                    if (unschedulingdid.at(pointer)->id == ops.at(count5)->id)
                    {
                        ops.at(count5)->scheduledState = (I);   // Assign that divider/modulo operation to the current latency cycle
                    }
                }

                schedulingdid.push_back(unschedulingdid.at(pointer));   // Push the pointed divider/modulo operation onto the scheduled queue
                unschedulingdid.erase(unschedulingdid.begin() + pointer);   // Remove the pointed divider/modulo operation from the unscheduled queue
            }
        }

        // ----------------------------------------------------------------

        // Decrease the schedule time for each resource by 1
        //
        for (int check1 = 0; check1 < schedulingadd.size(); check1++)
        {
            schedulingadd.at(check1)->scheduledTime -= 1;
        }
        for (int check1 = 0; check1 < schedulingmul.size(); check1++)
        {
            schedulingmul.at(check1)->scheduledTime -= 1;
        }
        for (int check1 = 0; check1 < schedulinglog.size(); check1++)
        {
            schedulinglog.at(check1)->scheduledTime -= 1;
        }
        for (int check1 = 0; check1 < schedulingdid.size(); check1++)
        {
            schedulingdid.at(check1)->scheduledTime -= 1;
        }
    }
}
