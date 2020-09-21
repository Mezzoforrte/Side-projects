int Snake::update_Snake(DIRECTION& new_dir, COORD old_Head, bool egg_eaten) {                           //! updates position of snake. TRUE if updates successfully.
                                                                    //! UNPOLISHED; NEEDS A SECOND LOOK.
            //! Updating direction container + BODY_COLLISION CHECK
    // Considerations:
    // 1) (For future me: why not update container before tail?
    //    IVE PROVEN THIS IN MY HEAD ALREADY, BUT SHORT LOGIC:
    //    "OLD_TAIL" NEEDS DATA FROM OLD_SNAKE; IF WE UPDATE SNAKE FIRST, THEN OLD_SNAKE NO LONGER EXISTS!!!
    // 2) This walks through the snake. Collision searching also requires walking through the snake
    //    while comparing the head coord to possible body coord. Thus, we AMORTIZE update (--value) + collision_check into a single loop.

    //     However, recall that we don't need to --(dir_count) if we ate the egg. How does this work then? Well, the chances of egg being
    //      eaten, in average case, is much less than the changes of not being eaten. Thus, we will take the tradeoff between iterating
    //      throught hte entire loop again to update with (++), over checking for a bool @ every single access in this initial iterating loop.



            // Pre-processing variables
    COORD prev_pivot = Head;


        // pushes new "turn segment" if not travelling in same direction as old one



    if (new_dir != dir_count.front().first && dir_count.size() > 2){        // SIMPLIFY .size() > 2
    }

    int count_update;                                   // we only decrement if egg does not get eaten
    if (egg_eaten){                                     // o/w keep everyting the same
        count_update = 0;
        length++;                                       // see proof C.
        dir_count.front().second++;                     //! TEST THIS
    }
    else
        count_update = -1;


        // Setting first pivot. Recall that updateHead() is called before updateBody(); this means a the latest input direction
        // is already the first element in dir_count.

    COORD pivot;

    pivot.X = Head.X;
    pivot.Y = Head.Y;

        //! DEBUGGING

        //! TEST THIS. Current state: dir_count.front().first
//    if (dir_count.front().first == DIRECTION::UP)
//        pivot.Y += static_cast<SHORT>(dir_count.front().second);
//    else if (dir_count.front().first == DIRECTION::DOWN)
//        pivot.Y += static_cast<SHORT>(dir_count.front().second);
//    else if (dir_count.front().first == DIRECTION::RIGHT)
//        pivot.X += static_cast<SHORT>(dir_count.front().second);
//    else if (dir_count.front().first == DIRECTION::LEFT)
//        pivot.X += static_cast<SHORT>(dir_count.front().second);

        // Walking the snake, starting with the SECOND container

    if (length == 1){

    }
    else {
      int current_It_length = length-1;        // this keeps track of how to decrement the pivot each iteration.

      for (typename QUEUE<std::pair<DIRECTION, int>>::iterator dir_count_it = ++dir_count.begin(); dir_count_it != dir_count.end(); dir_count_it++) {

          if (!(dir_count_it->second += count_update)) {      // updating the value BEFORE travelling just MAKES SENSE from a physical perspective.
             dir_count.pop_back();                            // (this check only matters when we're @ tail)
             break; // redundant?
          };

        std::cout << "hey" << std::endl;

        // COLLISION CANNOT OCCUR < 3RD TURN, REPRESENTED BY THE 4TH ELEMENT IN DIR_COUNT. ITERATIONS MAY BE BETTER.
        //     UPDATE: collissions CAN occurs on any iteration after the "4th" depending on whether we push the new head into the container or not.
        //              --> FOR NOW, just check each iteration. Can find an invariant after.
        //  TO START FROM THE TAIL END > HEAD.

        // Algorithm assuming we start form the TAIL instead of the HEAD



        // NOTE: very close to most optimal control flow. We CANNNOT start with pivot.x == head.x, because
        // we need to check for DIRECTION in cases of BOTH pivot.x === head.x AND pivot.x != pivot.x. Therefore, we eliminate
        // 1 comparison by structuring our flow this way.



        //! CONTROL FLOW SKELETAL CODE - NEEDS TO BE TESTED TO AMKE SURE X AND Y ARE TRUE.
        //!    However, pretty much as optimized as it's going to get.
            // not sure if X is appropriate for UP/DOWN. The COORDINATE SYSTEM for windows (X, Y) might be
            // reversed of cartesian.
            // Also, not sure if there is a single condition (where snake travels in same directiona s the dir_cont[x], that we can filter out.


        //! TEST THIS (second attempt; see proof on table)


        if (dir_count_it->first == DIRECTION::DOWN){
            int bound = static_cast<int>(pivot.Y) - current_It_length - dir_count_it->second -1; // bound set to upper-bound of x-value range=[bound, pivot.Y]
            if (Head.X == pivot.X){
                if (bound <= Head.Y && Head.Y <= pivot.Y)  // some math re: dir_count[i].second here
                    return(0);
            }
            current_It_length = dir_count_it->second;
            pivot.Y = bound;
        }

        else if (dir_count_it->first == DIRECTION::UP) {
            int bound = static_cast<int>(pivot.Y) + current_It_length - dir_count_it->second;        // bound set to upper-bound of x-value range=[bound, pivot.Y]
            if (Head.X == pivot.X){                         // body collision check here
                if (pivot.Y <= Head.Y && Head.Y <= bound)  // some math re: dir_count[i].second here
                    return(0);
            }
            current_It_length = dir_count_it->second;
            pivot.Y = bound;
        }


        else if (dir_count_it->first == DIRECTION::RIGHT){
            int bound = static_cast<int>(pivot.X) - current_It_length - dir_count_it->second;  // bound set to upper-bound of x-value range=[bound, pivot.X]
            if (Head.Y == pivot.Y){                                       // first have to check for possible y-collision...
                if (bound <= Head.X && Head.X <= pivot.X)                // if so, check  [pivot.X <= head.x <= newval]
                    return(0);
            }
            current_It_length = dir_count_it->second;
            pivot.X = bound;
        }

        else if (dir_count_it->first == DIRECTION::LEFT){
            int bound = static_cast<int>(pivot.X) + current_It_length - dir_count_it->second;  // bound set to upper-bound of x-value range=[bound, pivot.X]
            if (Head.Y == pivot.Y){                                       // first have to check for possible y-collision...
                if (pivot.X <= Head.X && Head.X <= bound)                 // if so, check  [pivot.X <= head.x <= newval]
                    return(0);
            }
            current_It_length = dir_count_it->second;
            pivot.X = bound;                                             // set new pivot for next iteration
        };
    };

        //! RESERVED FOR SPECIAL CASE to set pivot to tail via container data prior to the end iterator (SEE PROOF)
        //! TODO: modularize it.

        if (dir_count.back().first == DIRECTION::DOWN){
                            std::cout << "debug ASDFASDFASDFAD" << std::endl;

            int bound = static_cast<int>(pivot.Y) - current_It_length; // bound set to upper-bound of x-value range=[bound, pivot.Y]
            if (Head.X == pivot.X){
                if (bound <= Head.Y && Head.Y <= pivot.Y)  // some math re: dir_count[i].second here
                    return(0);
            }
            pivot.Y = bound;
        }

        else if (dir_count.back().first== DIRECTION::UP) {
                        std::cout << "debug ASDFASDF" << std::endl;

            int bound = static_cast<int>(pivot.Y) + current_It_length;        // bound set to upper-bound of x-value range=[bound, pivot.Y]
            if (Head.X == pivot.X){                        // body collision check here
                if (pivot.Y <= Head.Y && Head.Y <= bound)  // some math re: dir_count[i].second here
                    return(0);
            }
            pivot.Y = bound;
        }

        else if (dir_count.back().first == DIRECTION::RIGHT){
                        std::cout << "debug ASDFASDF" << std::endl;

            int bound = static_cast<int>(pivot.X) - current_It_length;  // bound set to upper-bound of x-value range=[bound, pivot.X]
            if (Head.Y == pivot.Y){                                       // first have to check for possible y-collision...
                if (bound <= Head.X && Head.X <= pivot.X)                // if so, check  [pivot.X <= head.x <= newval]
                    return(0);
            }
            pivot.X = bound;
        }

        else if (dir_count.back().first == DIRECTION::LEFT){
            std::cout << "debug ADSFASDF" << std::endl;
            int bound = static_cast<int>(pivot.X) + current_It_length;  // bound set to upper-bound of x-value range=[bound, pivot.X]
            if (Head.Y == pivot.Y){                                       // first have to check for possible y-collision...
                if (pivot.X <= Head.X && Head.X <= bound)                // if so, check  [pivot.X <= head.x <= newval]
                    return(0);
            }
            pivot.X = bound;                                             // set new pivot for next iteration
        };
    }

    std::cout << "   DIRCOUNT.FRONT(): ( " << static_cast<int>(dir_count.front().first) <<  " ,  "
    <<   static_cast<int>(dir_count.front().second) << ").   Cont size:   " << dir_count.size()
    << "  pivot.X: " << pivot.X << "  pivot.Y:  " << pivot.Y << "  newhead.X: " << Head.X << "  newHead.Y: " << Head.Y
    << "    " << '\r';


    Tail = pivot;           //! Note, this Tail is ONLY equal to pivot if walking the snake works properly.
                            //! otherwise, better just to do something like: if (egg_eaten) Tail = oldTail; else Tail = oldTail - w/e last container direction is.

    return(1);

};
