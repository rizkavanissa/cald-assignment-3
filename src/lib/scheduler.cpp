#include "../includes/scheduler.h"

void setALAPS(std::vector<Operation *> ops, int lat)
{
    int earliestSuc;
    ops.at(1)->setALAPTime(lat + 1);
    ops.at(1)->setALAPDone(true);

    while (!ops.at(0)->isALAPDone())
    {
        for (int i = 0; i < ops.size(); i++)
        {
            Operation *op = ops.at(i);
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
                    if (op->ALAPTime < 0)
                    {
                        std::cerr << "Error: Could Not ALAP schedule" << std::endl;
                    }
                }
            }
        }
    }
    return;
}

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

    //"Schedule In NOP"
    std::vector<Operation *> listing = ops;
    listing.erase(listing.begin(), listing.begin() + 2);

    std::vector<Operation *> unschedulingadd;
    std::vector<Operation *> unschedulingmul;
    std::vector<Operation *> unschedulingdid;
    std::vector<Operation *> unschedulinglog;

    std::vector<Operation *> schedulingadd;
    std::vector<Operation *> schedulingmul;
    std::vector<Operation *> schedulingdid;
    std::vector<Operation *> schedulinglog;

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

    used[Resource::ADDER] = 1;
    used[Resource::MULTIPLIER] = 1;
    used[Resource::LOGIC] = 1;
    used[Resource::DIVIDER] = 1;

    ifexist = false;

    for (int I = 1; I <= lat; I++)
    {
        // std::cout << "I =" << I << ":" << ifexist << "\n";
        /*for (int test1 = 0; test1 < listing.size(); test1++)
        {
            cout << "v" << listing.at(test1)->id << " edge is " << listing.at(test1)->getEdge() << " schetime is " << listing.at(test1)->scheduledTime <<  ".\n";
        }*/
        scheduledcomadd = false;
        scheduledcommul = false;
        scheduledcomdid = false;
        scheduledcomlog = false;
        // cout << "V5 is " << listing.at(4)->scheduledTime << "\n";

        /////////////////////////////check schetime,remove edge
        deladd = 0;
        delmul = 0;
        deldid = 0;
        dellog = 0;

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

        /////////check edge if is 0;
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

        /////////////////////////////////set scheduling ops
        // adder
        for (int count2 = 0; count2 < unschedulingadd.size(); count2++)
        {
            waitnum = unschedulingadd.at(count2)->ALAPTime - I;
            if (waitnum == 0)
            {
                unschedulingadd.at(count2)->scheduledTime = 1;
                schedulingadd.push_back(unschedulingadd.at(count2));
                used[ADDER] = schedulingadd.size();
                scheduledcomadd = true;
                for (int count5 = 0; count5 < ops.size(); count5++)
                {
                    if (unschedulingadd.at(count2)->id == ops.at(count5)->id)
                    {
                        ops.at(count5)->scheduledState = (I);
                    }
                }
            }
        }
        if (scheduledcomadd)
        {
            for (int check2 = 2; check2 < ops.size(); check2++)
            {
                if (ops.at(check2)->scheduledState != 0)
                {
                    for (int check3 = 0; check3 < unschedulingadd.size(); check3++)
                    {
                        if (unschedulingadd.at(check3)->id == ops.at(check2)->id)
                        {
                            unschedulingadd.erase(unschedulingadd.begin() + check3);
                        }
                    }
                }
            }
        }
        if (!scheduledcomadd && (unschedulingadd.size() > 0))
        {
            if (used[ADDER] >= schedulingadd.size())
            {
                int waitnum2 = lat;
                int pointer = 0;
                for (int count3 = 0; count3 < unschedulingadd.size(); count3++)
                {
                    waitnum = unschedulingadd.at(count3)->ALAPTime - I;
                    if (waitnum < waitnum2)
                    {
                        pointer = count3;
                        waitnum2 = waitnum;
                    }
                }
                unschedulingadd.at(pointer)->scheduledTime = 1;
                for (int count5 = 0; count5 < ops.size(); count5++)
                {
                    if (unschedulingadd.at(pointer)->id == ops.at(count5)->id)
                    {
                        ops.at(count5)->scheduledState = (I);
                    }
                }
                schedulingadd.push_back(unschedulingadd.at(pointer));
                unschedulingadd.erase(unschedulingadd.begin() + pointer);
            }
        }
        // muler
        for (int count2 = 0; count2 < unschedulingmul.size(); count2++)
        {
            waitnum = unschedulingmul.at(count2)->ALAPTime - I;
            if (waitnum == 0)
            {
                unschedulingmul.at(count2)->scheduledTime = 2;
                schedulingmul.push_back(unschedulingmul.at(count2));
                used[Resource::MULTIPLIER] = schedulingmul.size();
                scheduledcommul = true;
                for (int count5 = 0; count5 < ops.size(); count5++)
                {
                    if (unschedulingmul.at(count2)->id == ops.at(count5)->id)
                    {
                        ops.at(count5)->scheduledState = (I);
                    }
                }
            }
        }
        if (scheduledcommul)
        {
            for (int check2 = 2; check2 < ops.size(); check2++)
            {
                if (ops.at(check2)->scheduledState != 0)
                {
                    for (int check3 = 0; check3 < unschedulingmul.size(); check3++)
                    {
                        if (unschedulingmul.at(check3)->id == ops.at(check2)->id)
                        {
                            unschedulingmul.erase(unschedulingmul.begin() + check3);
                        }
                    }
                }
            }
        }
        if (!scheduledcommul && (unschedulingmul.size() > 0))
        {
            if (used[Resource::MULTIPLIER] > schedulingmul.size())
            {
                int waitnum2 = lat;
                int pointer = 0;
                for (int count3 = 0; count3 < unschedulingmul.size(); count3++)
                {
                    waitnum = unschedulingmul.at(count3)->ALAPTime - I;
                    if (waitnum < waitnum2)
                    {
                        pointer = count3;
                        waitnum2 = waitnum;
                    }
                }
                unschedulingmul.at(pointer)->scheduledTime = 2;
                for (int count5 = 0; count5 < ops.size(); count5++)
                {
                    if (unschedulingmul.at(pointer)->id == ops.at(count5)->id)
                    {
                        ops.at(count5)->scheduledState = (I);
                    }
                }
                schedulingmul.push_back(unschedulingmul.at(pointer));
                unschedulingmul.erase(unschedulingmul.begin() + pointer);
            }
        }

        // dider
        for (int count2 = 0; count2 < unschedulingdid.size(); count2++)
        {
            waitnum = unschedulingdid.at(count2)->ALAPTime - I;
            if (waitnum == 0)
            {
                unschedulingdid.at(count2)->scheduledTime = 3;
                schedulingdid.push_back(unschedulingdid.at(count2));
                used[Resource::DIVIDER] = schedulingdid.size();
                scheduledcomdid = true;
                for (int count5 = 0; count5 < ops.size(); count5++)
                {
                    if (unschedulingdid.at(count2)->id == ops.at(count5)->id)
                    {
                        ops.at(count5)->scheduledState = (I);
                    }
                }
            }
        }
        if (scheduledcomdid)
        {
            for (int check2 = 2; check2 < ops.size(); check2++)
            {
                if (ops.at(check2)->scheduledState != 0)
                {
                    for (int check3 = 0; check3 < unschedulingdid.size(); check3++)
                    {
                        if (unschedulingdid.at(check3)->id == ops.at(check2)->id)
                        {
                            unschedulingdid.erase(unschedulingdid.begin() + check3);
                        }
                    }
                }
            }
        }
        if (!scheduledcomdid && (unschedulingdid.size() > 0))
        {
            if (used[Resource::DIVIDER] > schedulingdid.size())
            {
                int waitnum2 = lat;
                int pointer = 0;
                for (int count3 = 0; count3 < unschedulingdid.size(); count3++)
                {
                    waitnum = unschedulingdid.at(count3)->ALAPTime - I;
                    if (waitnum < waitnum2)
                    {
                        pointer = count3;
                        waitnum2 = waitnum;
                    }
                }
                unschedulingdid.at(pointer)->scheduledTime = 3;
                for (int count5 = 0; count5 < ops.size(); count5++)
                {
                    if (unschedulingdid.at(pointer)->id == ops.at(count5)->id)
                    {
                        ops.at(count5)->scheduledState = (I);
                    }
                }
                schedulingdid.push_back(unschedulingdid.at(pointer));
                unschedulingdid.erase(unschedulingdid.begin() + pointer);
            }
        }

        // loger
        for (int count2 = 0; count2 < unschedulinglog.size(); count2++)
        {
            waitnum = unschedulinglog.at(count2)->ALAPTime - I;
            if (waitnum == 0)
            {
                unschedulinglog.at(count2)->scheduledTime = 1;
                schedulinglog.push_back(unschedulinglog.at(count2));
                used[LOGIC] = schedulinglog.size();
                scheduledcomlog = true;
                for (int count5 = 0; count5 < ops.size(); count5++)
                {
                    if (unschedulinglog.at(count2)->id == ops.at(count5)->id)
                    {
                        ops.at(count5)->scheduledState = (I);
                    }
                }
            }
        }
        if (scheduledcomlog)
        {
            for (int check2 = 2; check2 < ops.size(); check2++)
            {
                if (ops.at(check2)->scheduledState != 0)
                {
                    for (int check3 = 0; check3 < unschedulinglog.size(); check3++)
                    {
                        if (unschedulinglog.at(check3)->id == ops.at(check2)->id)
                        {
                            unschedulinglog.erase(unschedulinglog.begin() + check3);
                        }
                    }
                }
            }
        }
        if (!scheduledcomlog && (unschedulinglog.size() > 0))
        {

            if (used[LOGIC] > schedulinglog.size())
            {
                int waitnum2 = lat;
                int pointer = 0;
                for (int count3 = 0; count3 < unschedulinglog.size(); count3++)
                {
                    waitnum = unschedulinglog.at(count3)->ALAPTime - I;
                    if (waitnum < waitnum2)
                    {
                        pointer = count3;
                        waitnum2 = waitnum;
                    }
                }
                unschedulinglog.at(pointer)->scheduledTime = 1;
                for (int count5 = 0; count5 < ops.size(); count5++)
                {
                    if (unschedulinglog.at(pointer)->id == ops.at(count5)->id)
                    {
                        ops.at(count5)->scheduledState = (I);
                    }
                }
                schedulinglog.push_back(unschedulinglog.at(pointer));
                unschedulinglog.erase(unschedulinglog.begin() + pointer);
            }
        }
        //////////////////////////////////

        //////////////////////////////////////////////////////////////////////////////////

        ///////////

        for (int check1 = 0; check1 < schedulingadd.size(); check1++)
        {
            schedulingadd.at(check1)->scheduledTime -= 1;
        }
        for (int check1 = 0; check1 < schedulingmul.size(); check1++)
        {
            schedulingmul.at(check1)->scheduledTime -= 1;
        }
        for (int check1 = 0; check1 < schedulingdid.size(); check1++)
        {
            schedulingdid.at(check1)->scheduledTime -= 1;
        }
        for (int check1 = 0; check1 < schedulinglog.size(); check1++)
        {
            schedulinglog.at(check1)->scheduledTime -= 1;
        }
        /////////////////////////////////
    }
    // cout << "system need " << used[ADDER] << "adder, " << used[Resource::MULTIPLIER] << "mul, " << used[LOGIC] << " logics, " << used[Resource::DIVIDER] << " dids" << "\n";
    for (int test1 = 2; test1 < ops.size(); test1++)
    {
        // cout << "v" << ops.at(test1)->id << " lantancy clcye is " << ops.at(test1)->scheduledState << ".\n";
    }
}
