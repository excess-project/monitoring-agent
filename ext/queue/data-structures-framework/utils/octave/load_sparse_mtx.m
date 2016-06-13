%% Loads a sparse matrix in MatrixMarket general format.
%%
%% Copyright (C) 2016  Anders Gidenstam, Chalmers University of Technology
%%

function [M m n M_triplets]= load_sparse_mtx(filename)
  M_triplets = load('-ascii', filename);
  m = M_triplets(1,1);
  n = M_triplets(1,2);
  % Strip off the line with the dimensions and nnz.
  M_triplets = M_triplets(2:rows(M_triplets),:);
  M = sparse(M_triplets(:,1), M_triplets(:,2), M_triplets(:,3), m, n, 'unique');
endfunction
