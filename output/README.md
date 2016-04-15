OCA Output Directory
==========
Once 'oca2fit' has been called the results of the fit stored here. The fitted parameters are saved in the 'fits' directory. 

For convenience, all the PMTs from all runs included in a fit are saved onto a single file known as a datastore. For each fit, two datastores are written an 'unfiltered' and 'filtered' datastore. The 'unfiltered' datastore contains all of the PMTs from all runs included in the fit prior to data selection cuts. The 'filtered' contains only those PMTs which remained at the end of the fit after all selection criteria had been applied. 
