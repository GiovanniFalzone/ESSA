OBJ_DIR  := obj
OBJ_EXTENSION := o
SRC_EXTENSION := c
SRC_DIR := testedCode/
PROG_DIR := progs/
INC_DIR := inc/

test_ESSTA_JSON_lib: $(OBJ_DIR)/test_ESSTA_JSON_lib.o $(OBJ_DIR)/ESSTA_JSON_lib.o
	gcc --static $^ -L /usr/local/lib/ -lcunit  -o $@

$(OBJ_DIR)/%.$(OBJ_EXTENSION) : $(SRC_DIR)/%.$(SRC_EXTENSION) 
	gcc -c $< -Iinc/ -o $@
$(OBJ_DIR)/%.$(OBJ_EXTENSION) : $(PROG_DIR)/%.$(SRC_EXTENSION) 
	gcc -c $< -Iinc/ -o $@

clean:
	rm -rf test_ESSTA_JSON_lib obj/*.o *~ $(SRC_DIR)/*~ $(PROG_DIR)/*~
