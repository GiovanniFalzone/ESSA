OBJ_DIR  := obj
OBJ_EXTENSION := o
SRC_EXTENSION := c
SRC_DIR := testedCode/
PROG_DIR := progs/
INC_DIR := inc/

test_globe: $(OBJ_DIR)/test_globe.o $(OBJ_DIR)/globe.o 
	gcc --static $^ -L /usr/local/lib/ -lcunit  -o $@

$(OBJ_DIR)/%.$(OBJ_EXTENSION) : $(SRC_DIR)/%.$(SRC_EXTENSION) 
	gcc -c $< -Iinc/ -o $@
$(OBJ_DIR)/%.$(OBJ_EXTENSION) : $(PROG_DIR)/%.$(SRC_EXTENSION) 
	gcc -c $< -Iinc/ -o $@

clean:
	rm -rf test_globe obj/*.o *~ $(SRC_DIR)/*~ $(PROG_DIR)/*~
