% find best weak learner and its error
function [index_j,thresh_j,parity_j,min_error_j] = findBestWeakLearner(train_data,train_labels,Dt);

% No of training samples & dimensionalty of feature vector
[N,d] = size(train_data);

min_error_j = 10000;
for j=1:d
    % sort feature column
    [sort_feature,sort_index] = sort(train_data(:,j));
    sort_labels = train_labels(sort_index(:));
    sort_weights = Dt(sort_index(:));
    
    min_error_th = 10000;
    for th=1:N-1
        % find sum of positive & negative sample weights BELOW threshold
        SUM_POS_WT_BELOW = 0;
        SUM_NEG_WT_BELOW = 0;
        for i=1:th
            if(sort_labels(i) == 1)
                SUM_POS_WT_BELOW = SUM_POS_WT_BELOW + sort_weights(i);
            elseif(sort_labels(i) == -1)
                SUM_NEG_WT_BELOW = SUM_NEG_WT_BELOW + sort_weights(i);
            end
        end
        % find TOTAL sum of positive & negative sample weights
        SUM_POS_WT_TOTAL = 0;
        SUM_NEG_WT_TOTAL = 0;
        for i=1:N
            if(sort_labels(i) == 1)
                SUM_POS_WT_TOTAL = SUM_POS_WT_TOTAL + sort_weights(i);
            elseif(sort_labels(i) == -1)
                SUM_NEG_WT_TOTAL = SUM_NEG_WT_TOTAL + sort_weights(i);
            end
        end
        % find sum of positive & negative sample weights ABOVE threshold
        SUM_POS_WT_ABOVE = SUM_POS_WT_TOTAL - SUM_POS_WT_BELOW;
        SUM_NEG_WT_ABOVE = SUM_NEG_WT_TOTAL - SUM_NEG_WT_BELOW;
        
        % check for both parity
        error_th = min(SUM_POS_WT_BELOW+SUM_NEG_WT_ABOVE , SUM_NEG_WT_BELOW+SUM_POS_WT_ABOVE);
        if(SUM_NEG_WT_BELOW+SUM_POS_WT_ABOVE < SUM_POS_WT_BELOW+SUM_NEG_WT_ABOVE)
            parity_tmp = -1;
        else
            parity_tmp = 1;
        end
        
        % find min error out of all threshold values
        if(error_th < min_error_th)
            min_error_th = error_th;
            %index_th_sort = th;
            thresh_th = (sort_feature(th) + sort_feature(th+1))/2;
            parity_th = parity_tmp;
        end
    end
    
    % find min error out of all feature columns
    if(min_error_th < min_error_j)
        min_error_j = min_error_th;
        index_j = j;
        thresh_j = thresh_th;
        parity_j = parity_th;
    end
end
end
